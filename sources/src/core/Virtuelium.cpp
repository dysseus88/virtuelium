/*
 *  Copyright 2013 Remi "Programmix" Cerise
 *
 *  This file is part of Virtuelium.
 *
 *  Virtuelium is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */
#include <core/Virtuelium.hpp>
//!
//! @file Virtuelium.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements the class declared in 
//!  Virtuelium.hpp 
//!  @arg Virtuelium
//! @todo 
//! @remarks 
//!
#include <iostream>
#include <fstream>

#include <omp.h>
#include <tclap/CmdLine.h>
#include <tclap/Constraint.h>

#include <core/VrtLog.hpp>
#include <core/taskexecutor/TaskExecutorBase.hpp>
#include <core/taskexecutor/StandAloneExecutor.hpp>
#include <core/taskexecutor/ClientServerExecutor.hpp>
////////////////////////////////////////////////////////////////////////////////
Virtuelium::Virtuelium(int argc, char* argv[]) 
    : m_mpi_rank(0), 
      m_nb_mpi_procs(1),
      m_nb_omp_procs(1),
      m_xmin(-1), 
      m_ymin(-1),
      m_xmax(-1),
      m_ymax(-1),
      m_tsk_w(100),
      m_tsk_h(100),
      b_debug(false),
      b_brdf(false),
      m_brdf_step(-1),
      b_overwrite(false),
      b_fragment(false),
      m_scenery_filename(""),
      p_scenery(NULL),
      p_exec(NULL),
      m_algorithm(""),
      m_nb_task_refresh(5),
      m_chunk(-1),
      m_save_init_file(""),
      m_load_init_file("") {

  MPI::Init(argc, argv);
  m_mpi_rank = MPI::COMM_WORLD.Get_rank();
  m_nb_mpi_procs = MPI::COMM_WORLD.Get_size();

  m_algo_params.clear();
}
////////////////////////////////////////////////////////////////////////////////
Virtuelium::~Virtuelium(void) {
  if (p_exec != NULL) {
    delete p_exec;
    p_exec = NULL;
  }
  if (p_scenery != NULL) {
    delete p_scenery ;
    p_scenery  = NULL;
  }

  MPI::Finalize();
  VrtLog::Close();
}
////////////////////////////////////////////////////////////////////////////////
bool Virtuelium::ParseAreaParam(const char* area_str, 
                                int& xmin, int& ymin, int& xmax, int& ymax) {
  std::string s = area_str;
  
  size_t pos = s.find_first_of(":");
  if (pos == std::string::npos) 
    return false;
  xmin = strtol(s.substr(0, pos).c_str(), NULL, 10);
  if (xmin < 0)
    return false;
  s = s.substr(pos + 1);

  pos = s.find_first_of(":");
  if (pos == std::string::npos)
    return false;
  ymin = strtol(s.substr(0, pos).c_str(), NULL, 10);
  if (ymin < 0)
    return false;
  s = s.substr(pos + 1);

  pos = s.find_first_of(":");
  if (pos == std::string::npos)
    return false;
  xmax = strtol(s.substr(0, pos).c_str(), NULL, 10);
  if (xmax < 0)
    return false;
  s = s.substr(pos + 1);

  ymax = strtol(s.c_str(), NULL, 10);
  if (ymax < 0)
    return false;

  int tmp = 0;
  if (xmin  > xmax) {
    tmp = xmin;
    xmin = xmax;
    xmax = tmp;
  }
  if (ymin  > ymax) {
    tmp = ymin;
    ymin = ymax;
    ymax = tmp;
  }

  if (xmax == xmin || ymax == ymin)
    return false;

  return true;
}
////////////////////////////////////////////////////////////////////////////////
void Virtuelium::ParseAlgoParam(const std::string& str_line,
                                const std::string& str_line_snake,
                                bool spiral_trigo, bool spiral_inverse) {                                 
  // Line 
  if (str_line.compare("LRTB") == 0) {
    m_algorithm = "Line";
    m_algo_params.push_back(TaskManagerLine::kLRTB);
  } else if (str_line.compare("LRBT") == 0) {
    m_algorithm = "Line";
    m_algo_params.push_back(TaskManagerLine::kLRBT);
  } else if (str_line.compare("RLTB") == 0) {
    m_algorithm = "Line";
    m_algo_params.push_back(TaskManagerLine::kRLTB);
  } else if (str_line.compare("RLBT") == 0) {
    m_algorithm = "Line";
    m_algo_params.push_back(TaskManagerLine::kRLBT);
  } else if (str_line.compare("TBLR") == 0) {
    m_algorithm = "Line";
    m_algo_params.push_back(TaskManagerLine::kTBLR);
  } else if (str_line.compare("TBRL") == 0) {
    m_algorithm = "Line";
    m_algo_params.push_back(TaskManagerLine::kTBRL);
  } else if (str_line.compare("BTLR") == 0) {
    m_algorithm = "Line";
    m_algo_params.push_back(TaskManagerLine::kBTLR);
  } else if (str_line.compare("BTRL") == 0) {
    m_algorithm = "Line";
    m_algo_params.push_back(TaskManagerLine::kBTRL);

    // Line with snaking
  } else if (str_line_snake.compare("LRTB") == 0) {
    m_algorithm = "Line";
    m_algo_params.push_back(TaskManagerLine::kLRTB);
    m_algo_params.push_back(TaskManagerLine::kSNAKE);
  } else if (str_line_snake.compare("LRBT") == 0) {
    m_algorithm = "Line";
    m_algo_params.push_back(TaskManagerLine::kLRBT);
    m_algo_params.push_back(TaskManagerLine::kSNAKE);
  } else if (str_line_snake.compare("RLTB") == 0) {
    m_algorithm = "Line";
    m_algo_params.push_back(TaskManagerLine::kRLTB);
    m_algo_params.push_back(TaskManagerLine::kSNAKE);
  } else if (str_line_snake.compare("RLBT") == 0) {
    m_algorithm = "Line";
    m_algo_params.push_back(TaskManagerLine::kRLBT);
    m_algo_params.push_back(TaskManagerLine::kSNAKE);
  } else if (str_line_snake.compare("TBLR") == 0) {
    m_algorithm = "Line";
    m_algo_params.push_back(TaskManagerLine::kTBLR);
    m_algo_params.push_back(TaskManagerLine::kSNAKE);
  } else if (str_line_snake.compare("TBRL") == 0) {
    m_algorithm = "Line";
    m_algo_params.push_back(TaskManagerLine::kTBRL);
    m_algo_params.push_back(TaskManagerLine::kSNAKE);
  } else if (str_line_snake.compare("BTLR") == 0) {
    m_algorithm = "Line";
    m_algo_params.push_back(TaskManagerLine::kBTLR);
    m_algo_params.push_back(TaskManagerLine::kSNAKE);
  } else if (str_line_snake.compare("BTRL") == 0) {
    m_algorithm = "Line";
    m_algo_params.push_back(TaskManagerLine::kBTRL);
    m_algo_params.push_back(TaskManagerLine::kSNAKE);

    // spiral 
  } else if (spiral_trigo) {
    m_algorithm = "Spiral";
    m_algo_params.push_back(TaskManagerSpiral::kTRIGO);
  } else if (spiral_inverse) {
    m_algorithm = "Spiral";
    m_algo_params.push_back(TaskManagerSpiral::kNONTRIGO);

    // default case = LKRTB without snaking
  } else {
    m_algorithm = "Line";
    m_algo_params.push_back(TaskManagerLine::kLRTB);
  }
}
////////////////////////////////////////////////////////////////////////////////
void Virtuelium::ReadParameters(int argc, char* argv[]) {
  try{
    // Use TCLAP to parse the command line arguments
    TCLAP::CmdLine cmd("\n\
======================================================================== \
==============================  VIRTUELIUM  ============================ \
======================================================================== \
\n\
Virtuelium is a full spectral rendering engine that can be deployed on massive \
parralel archirtectures. More info can be found in the 'doc' folder. In order \
to use it, you first have to describe your 3D scene in the appropriate xml \
format.", ' ', "5.0.0");
  //////////////////////////////////////////////////////////////////////

    // Save rendering initialization maps
    TCLAP::ValueArg<std::string> arg_save_init("", "save-init", 
"Save the rendering initiliation maps in a binary file.",
false, "","string", cmd);

    // Load rendering initialization maps
    TCLAP::ValueArg<std::string> arg_load_init("", "load-init", 
"Load the rendering initiliation maps in a binary file.",
false, "","string", cmd);

    // Algorithms (constraint value)
    std::vector<std::string> allowed_line_str;
    allowed_line_str.push_back("LRTB");
    allowed_line_str.push_back("LRBT");
    allowed_line_str.push_back("RLTB");
    allowed_line_str.push_back("RLBT");
    allowed_line_str.push_back("TBLR");
    allowed_line_str.push_back("TBRL");
    allowed_line_str.push_back("BTLR");
    allowed_line_str.push_back("BTRL");
    TCLAP::ValuesConstraint<std::string> allowed_line_args(allowed_line_str);
    // Algorithms definition
    TCLAP::ValueArg<std::string> arg_line("", "line",
"Use the Line Algorithm. The image will be renderer line by line or column by \
column, respecting the order given the value of the argument. \
This value can be: \
LRTB (from left to right, then from top to bottom), \
LRBT (from left to right, then from bottom to top), \
RLTB (from right to left, then from top to bottom), \
RLBT (from right to left, then from bottom to top), \
TBLR (from top to bottom, then from left to right), \
TBRL (from top to bottom, then from right to left), \
BTLR (from bottom to top, then from left to right), \
BTRL (from bottom to top, then from right to left). Once an entire line or \
column has been rendered, the following line is rendered in the SAME order.",
false, "", &allowed_line_args, cmd);
    TCLAP::ValueArg<std::string> arg_line_snake("", "line-snake",
"Use the Line Algorithm. The image will be renderer line by line or column by \
column, respecting the order given the value of the argument. \
This value can be: \
LRTB (from left to right, then from top to bottom), \
LRBT (from left to right, then from bottom to top), \
RLTB (from right to left, then from top to bottom), \
RLBT (from right to left, then from bottom to top), \
TBLR (from top to bottom, then from left to right), \
TBRL (from top to bottom, then from right to left), \
BTLR (from bottom to top, then from left to right), \
BTRL (from bottom to top, then from right to left). Once an entire line or \
column has been rendered, the following line is rendered in the INVERSE order.",
false, "", &allowed_line_args, cmd);
    TCLAP::SwitchArg arg_spiral_trigo("", "spiral",
"Use the Spiral Algorithm. The image will be renderer by following a \
trigonometric spiral pattern.", cmd, false);
    TCLAP::SwitchArg arg_spiral_inverse("", "spiral-inverse",
"Use the Spiral Algorithm. The image will be renderer by following a \
non-trigonometric spiral pattern.", cmd, false);

    // Fragmented mode
    TCLAP::SwitchArg arg_fragment_mode("", "fragment", 
"Fragmented image. Each node will produce parts of the image in separate files \
instead of building a global output file. Names of each output give info about \
the sub-image position in the virtual global image. This option is only  used \
by the Client-Server mode. In Stand-Alone mode, the image is always built in \
a single file.",
cmd, false);

    // Height of a task unit
    TCLAP::ValueArg<int> arg_tsk_h("v", "tsk_height", 
"Height of a task unit.",
false, 100,"integer", cmd);

    // Width of task unit
    TCLAP::ValueArg<int> arg_tsk_w("u", "tsk_width", 
"Width of task unit.",
false, 100,"integer", cmd);

    // Override mode
    TCLAP::SwitchArg arg_overwrite_mode("o", "overwrite", 
"Overwrite the previous image file. This option is only used by the Stand-Alone\
 mode. In Client-Server mode, the image is always overwritten.",
cmd, false);

    // Number of openMP processes by node
    TCLAP::ValueArg<int> arg_nb_omp_threads("n", "nb-threads", 
"Number of processes by node. In Stand-Alone execution (without mpiexec), \
there is only one computing node, otherwise the number of computing nodes is \
determined by the -n argument of the mpiexec command.",
false, omp_get_num_procs(),"integer", cmd);

    // Frequency
    TCLAP::ValueArg<unsigned int> arg_frequency("f", "save-frequency", 
"Number of tasks to be rendered between two saving operations. This option is \
only used by the Stand-Alone mode. In Client-Server mode, the image is save at \
the end of each block (if fragmented) or at the end of the computation.",
false, 5,"integer", cmd);

    // Debug mode
    TCLAP::SwitchArg arg_debug_mode("d", "debug", 
"Write debug info into log files.",
cmd, false);

    // Image chunk
    TCLAP::ValueArg<int> arg_chunk("c", "image-chunk", 
      "Size of a computing bloc. The image to be rendered is decomposed in two \
ways. The smallest area rendered by one process is called a task unit (see -u \
and -v arguments in order to control the size of a task unit). A computing \
block is a set of task units that is send to a computing node (a computer) in \
one time. In Stand-Alone execution (without mpiexec), there is only one \
computing node, otherwise the number of computing nodes is determined by the -n\
 argument of the mpiexec command.",
false, -1, "integer", cmd);

    // BRDF creator
    TCLAP::ValueArg<int> arg_brdf("b", "brdf-creator", 
"Create BRDF files for each surface of the scene instead of rendering them. The\
 associated argument determines the sampling step (angular length in degrees). \
Set to 0, for irregular sampling.",
false, -1, "integer", cmd);

    // Area of the image to be rendered
    TCLAP::ValueArg<std::string> arg_area("a", "area", 
"Only compute this sub-area of the image. By default, the whole image will be \
rendered.\nBBOX format: xmin:ymin:xmax:ymax",
false, "", "string", cmd);

    // XML scene file
    TCLAP::UnlabeledValueArg<std::string> arg_scenefile("scenery",
"The XML scenery file describing the 3D scene to be rendered. See Virtuelium \
documentation and examples to write your scenery file or use export plugins.", 
true, "", "string", cmd);


  //////////////////////////////////////////////////////////////////////

    // Parse the command line
    cmd.parse(argc, argv);   
  //////////////////////////////////////////////////////////////////////

    // Retrieve the scenery
    m_scenery_filename = arg_scenefile.getValue();

    // Retrieve the sub-area to be rendered
    std::string area = arg_area.getValue();
    if (! area.empty()) {
      if (! ParseAreaParam(area.c_str(), m_xmin, m_ymin, m_xmax, m_ymax)) {
        throw TCLAP::ArgParseException("Wrong or does not match format: \
xmin:ymin:xmax:ymax", "area");
      }
    }

    // Retrieve the BRDF creator mode
    m_brdf_step = arg_brdf.getValue();
    if (m_brdf_step > 0)
      b_brdf = true;

    // Retrieve the image chunk
    m_chunk = arg_chunk.getValue();

    // Retrieve the debug mode
    b_debug  = arg_debug_mode.getValue();

    // Retrieve the save frequency
    m_nb_task_refresh = arg_frequency.getValue();

    // Retrieve the number of processes by node
    m_nb_omp_procs = arg_nb_omp_threads.getValue();  

    // Retrieve the overwrite and fragment modes
    b_overwrite  = arg_overwrite_mode.getValue();
    b_fragment  = arg_fragment_mode.getValue();
    if (b_fragment)
      b_overwrite = true;

    // Retrieve size of a task unit
    m_tsk_w = arg_tsk_w.getValue();
    m_tsk_h = arg_tsk_h.getValue();

    // Retrieve the algorithm pattern
    ParseAlgoParam(arg_line.getValue(), arg_line_snake.getValue(),
                    arg_spiral_trigo.getValue(), arg_spiral_inverse.getValue());
    
    // Retrieve the save / load for rendering inits
    m_save_init_file = arg_save_init.getValue();
    m_load_init_file = arg_load_init.getValue();

      // Catch any exceptions
  } catch (TCLAP::ArgException &e) { 
    std::cerr << "Error: " << e.error() 
              << " for " << e.argId() << std::endl; 
    exit(0);
  }
}
////////////////////////////////////////////////////////////////////////////////
void Virtuelium::CheckDebugMode(void) {
  if (b_debug == false)
    return;

  VrtLog::Create(m_mpi_rank, "log.txt");
  VrtLog::Write("----- m_mpi_rank: %d", m_mpi_rank);
  VrtLog::Write("----- m_nb_mpi_procs: %d", m_nb_mpi_procs);
  VrtLog::Write("----- m_nb_omp_procs: %d", m_nb_omp_procs);
  VrtLog::Write("----- Area: (%d, %d) - (%d, %d)", m_xmin, m_ymin, 
                                                   m_xmax, m_ymax);
  VrtLog::Write("----- Task Unit: (%d, %d))", m_tsk_w, m_tsk_h); 
  VrtLog::Write("----- Debug mode: %d", b_debug);
  VrtLog::Write("----- BRDR creator mode: %d (step = %d)", b_brdf, m_brdf_step);
  VrtLog::Write("----- Override mode: %d", b_overwrite);
  VrtLog::Write("----- Fragmented mode: %d", b_fragment);
  VrtLog::Write("----- Scenery: %s", m_scenery_filename.c_str());
  VrtLog::Write("----- Algorithm: %s with parameters:", m_algorithm.c_str());
  for (unsigned int i = 0; i < m_algo_params.size(); i++) 
    VrtLog::Write("        %d", m_algo_params[i]);
  VrtLog::Write("----- m_nb_task_refresh: %u", m_nb_task_refresh);
}
////////////////////////////////////////////////////////////////////////////////
void Virtuelium::InitializeScenery(void) {
  //Loading the scenery and check area
  if (m_mpi_rank == 0)	{
    std::cout << "Chargement de la scène ("<< m_scenery_filename <<") :";
  }
  if (p_scenery != NULL) {
    delete p_scenery;
    p_scenery = NULL;
  }

  GlobalSceneryParser parser;
  p_scenery = parser.ParseAndBuildScenery(m_scenery_filename);

  int width = p_scenery->getCamera(0)->getWidth();
  int height = p_scenery->getCamera(0)->getHeight();
  if (m_xmin < 0) m_xmin = 0;
  if (m_ymin < 0) m_ymin = 0;
  if (m_xmax < 0 || m_xmax >= width) m_xmax = width - 1;
  if (m_ymax < 0 || m_ymax >= height) m_ymax = height - 1;

  if (m_mpi_rank == 0) {
    std::cout << " [ OK ]" << std::endl;
  }
}
////////////////////////////////////////////////////////////////////////////////
void Virtuelium::InitializeRenderer(void) {
  if (m_mpi_rank == 0)	{
    if (p_scenery != NULL) {
      std::cout << "Initialisation du moteur de rendu :";
      unsigned int data_size = 0;
      unsigned char* data = NULL;
      
      // Try to load previous rendering init from a file
      if (! m_load_init_file.empty()) {
        std::cout << "m_load_init_file :" << m_load_init_file << std::endl;
        LoadInit(&data, &data_size);
        std::cout << "data :" << data << std::endl;
        std::cout << "data_size :" << data_size << std::endl;
        p_scenery->getRenderer()->InitWithData(*p_scenery, data, data_size );
      } else {
        p_scenery->getRenderer()->Init(*p_scenery, 1);
      }

      // Try to save the rendering inti into a file
      if (! m_save_init_file.empty()) {
       std::cout << "m_save_init_file :" << m_save_init_file << std::endl;
       p_scenery->getRenderer()->ExportData(&data, &data_size);
       SaveInit(data, data_size);
      }

      std::cout << " [ OK ]" << std::endl;
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
void Virtuelium::SaveInit(unsigned char* data, unsigned int data_size) {
  std::ofstream ofs;
  ofs.open(m_save_init_file.c_str(), std::ios::binary);
  if (! ofs.is_open()) 
    throw Exception("(Virtuelium::SaveInit)Sauvegarde des données impossible.");
 
  ofs.write(reinterpret_cast<const char*> (&data_size), 
            sizeof(unsigned int));
  ofs.write(reinterpret_cast<const char*> (data), 
            data_size * sizeof(unsigned char));
  ofs.close();
}
////////////////////////////////////////////////////////////////////////////////
void Virtuelium::LoadInit(unsigned char** data, unsigned int* data_size) {
  std::ifstream ifs;
  ifs.open(m_load_init_file.c_str(), std::ios::binary);
  if (! ifs.is_open()) 
    throw Exception("(Virtuelium::LoadInit)Chargement des données impossible.");
 
  ifs.read(reinterpret_cast<char*> (data_size), 
           sizeof(unsigned int));

  *data = new unsigned char[*data_size];
  ifs.read(reinterpret_cast<char*> (*data), 
           *data_size * sizeof(unsigned char));

  printf("size=%u", *data_size);
  ifs.close();
}
////////////////////////////////////////////////////////////////////////////////
void Virtuelium::InitializeExecutor(void) {
  if (m_mpi_rank == 0)  {
    std::cout << "Initialisation du lancer de rayons ";
    std::cout.flush();
  }

  // One MPI process => stand alone execution with openMP
  if (m_nb_mpi_procs == 1) {
    if (m_mpi_rank == 0) 
      std::cout << "(Stand-Alone) : ";
    p_exec = new StandAloneExecutor;
    p_exec->SetBaseParameters(m_xmin, m_ymin, m_xmax, m_ymax, 
                              m_tsk_w, m_tsk_h, m_nb_task_refresh,
                              b_overwrite, m_chunk);
    p_exec->SetAdditionalParameters(1, m_nb_omp_procs);
    
    if (m_algo_params.size() == 1) {
      p_exec->Initialize(p_scenery, m_algorithm, 1, 
                         m_algo_params[0]);
    } else if (m_algo_params.size() == 2) {
      p_exec->Initialize(p_scenery, m_algorithm, 2, 
                         m_algo_params[0], m_algo_params[1]);
    }

  // Several MPI process => hybrid client-server execution MPI-openMP
  } else if (m_nb_mpi_procs > 1) {
    if (m_mpi_rank == 0)
      std::cout << "(Client-Server) :";
    p_exec = new ClientServerExecutor;
    p_exec->SetBaseParameters(m_xmin, m_ymin, m_xmax, m_ymax, 
                              m_tsk_w, m_tsk_h, m_nb_task_refresh,
                              b_overwrite, m_chunk);
    p_exec->SetAdditionalParameters(3, m_mpi_rank, m_nb_mpi_procs, 
                                    m_nb_omp_procs);
    
    if (m_algo_params.size() == 1) {
      p_exec->Initialize(p_scenery, m_algorithm, 1, 
                         m_algo_params[0]);
    } else if (m_algo_params.size() == 2) {
      p_exec->Initialize(p_scenery, m_algorithm, 2, 
                         m_algo_params[0], m_algo_params[1]);
    }
  }

  if (m_mpi_rank == 0)  {
    std::cout << " [ OK ]" << std::endl;
  }
}
////////////////////////////////////////////////////////////////////////////////
void Virtuelium::Execute(void) {
  if (m_mpi_rank == 0)  {
    std::cout << "Exécution du moteur de rendu :" << std::endl;
  }
  
  p_exec->Execute();

  if (m_mpi_rank == 0)  {
    std::cout << " [ FIN ]" << std::endl;
  }
}
////////////////////////////////////////////////////////////////////////////////
void Virtuelium::TraceBRDF(const int& step) {
}
////////////////////////////////////////////////////////////////////////////////
