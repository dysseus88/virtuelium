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
//!
//! @file main.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This is the main file of the Virtuelium program
//!
#include <core/Virtuelium.hpp>
////////////////////////////////////////////////////////////////////////////////
//! @brief Set up the Linux signal handeling
void handleSignal(void) {
#ifndef WIN32
  struct sigaction signalHandler;
  signalHandler.sa_handler = SIG_IGN;
  signalHandler.sa_flags = 0;
  sigemptyset(&signalHandler.sa_mask);
  sigaction(SIGPIPE, &signalHandler, 0); //SIGPIPE = 13
#endif
}
////////////////////////////////////////////////////////////////////////////////
void PrintDateAndTime(int rank) {
  if (rank != 0)
    return;

  time_t now = time(0);
  struct tm tstruct;
  char buf[80];
  tstruct = *localtime(&now);

  strftime(buf, sizeof(buf), "\n ---------- %Y-%m-%d ---- %X ----------\n",
           &tstruct);

  std::cout << buf << std::endl;
}

//#include <boost/archive/binary_iarchive.hpp>
//#include <boost/archive/binary_oarchive.hpp>
//using namespace boost::archive;
//
//class A {
//public:
//  A(void):x(10) {}
//  A(int xx):x(xx) {}
//  void print(void) {
//    std::cout << "\n x = " << x << std::endl;
//  }
//private:
// friend class boost::serialization::access;
// template<class Archive>
// void serialize(Archive& ar, const unsigned int version) {
//  ar & x;
// }
//private:
//  int x;
//};

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {
  // Parser parameters
  Virtuelium vrt(argc, argv);
  vrt.ReadParameters(argc, argv);

  // Start chronometer
  PrintDateAndTime(vrt.mpi_rank());
  time_t starttime = time(NULL);
  srand((unsigned int)starttime);
  handleSignal();

//  A a(50);
//  std::ofstream ofile("test.bin");
//  boost::archive::binary_oarchive oBinArchive(ofile);
//  oBinArchive << a;
//  ofile.close();
//
//  A b;
//  std::ifstream ifile("test.bin");
//  boost::archive::binary_iarchive iBinArchive(ifile);
//  iBinArchive >> b;
//  ifile.close();
//  b.print();

  try{
    // Start debug if necessary
    vrt.CheckDebugMode();
    // Initialize the scenery
    vrt.InitializeScenery();

    // BRDF creator mode is ON
    if (vrt.brdf()) {
      vrt.TraceBRDF(vrt.brdf_step());

    // BRDF creator mode if OFF
    } else {
      // Initialize Renderer
      vrt.InitializeRenderer();
      // Initialize the image computing
      vrt.InitializeExecutor();
      // Execute
      vrt.Execute();
    }

  // Error managment
  } catch(Exception exc) {
    if (vrt.mpi_rank() == 0) {
      std::cout << std::endl
                << "    [" << vrt.mpi_rank() << "] "
                << "Une exception a été levée et le programme a été interrompu."
                << std::endl
                << "              "
                << "Voici le message l'exception levée : "
                << std::endl
                << "              "
                << exc
                << std::endl << std::endl;
    }
    return -1;
  }
  ///////////////////////////
  // end message
  if (vrt.mpi_rank() == 0) {
    std::cout << std::endl
      << "Rendu terminé. Temps de rendu total : "
      << (time(NULL)-starttime) << " secondes. "
      << std::endl << std::endl;
  }

  PrintDateAndTime(vrt.mpi_rank());
  return 0;
}
////////////////////////////////////////////////////////////////////////////////
