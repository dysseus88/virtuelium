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
#include <core/taskexecutor/StandAloneExecutor.hpp>
//!
//! @file StandAloneExecutor.cpp
//! @author Remi "Programmix" Cerise
//! @version 5.0.0
//! @date 2013
//! @details This file implements classs declared in StandAloneExecutor.hpp 
//!  @arg StandAloneExecutor
//!
#include <cstdarg>
#include <omp.h>

#include <core/VrtLog.hpp>

#include <core/Scenery.hpp>
#include "io/image/ImageParser.hpp"
#include <structures/Image.hpp>
#include <exceptions/Exception.hpp>
////////////////////////////// class StandAloneExecutor //////////////////////////
StandAloneExecutor::StandAloneExecutor(void)
    : m_nb_openmp_process(1) {}
////////////////////////////// class StandAloneExecutor //////////////////////////
StandAloneExecutor::~StandAloneExecutor(void) {
}
////////////////////////////// class StandAloneExecutor //////////////////////////
void StandAloneExecutor::SetAdditionalParameters(int nb_params, ...) { 
  va_list(params);
  va_start(params, nb_params);

  if (nb_params == 1) {
    m_nb_openmp_process = va_arg(params, int);
  
  // Bad number of parameters
  } else {
   va_end(params);
   throw Exception("(StandAloneExecutor::SetAdditionalParameters) Nombre \
incorrect de paramètres");
  }

  va_end(params);
}
////////////////////////////// class StandAloneExecutor //////////////////////////
void StandAloneExecutor::Initialize(Scenery* scenery, 
                                    const std::string& manager_class,
                                    int nb_params, ...) {
  EraseTaskManager();

  // Scenery
  p_scenery = scenery;

  // Line Algorithm 
  if (manager_class.compare("Line") == 0) {
    p_task_mngr = new TaskManagerLine;

  // Spiral Algorithm
  } else if (manager_class.compare("Spiral") == 0) {
    p_task_mngr = new TaskManagerSpiral;

  // Unknown
  } else {
    throw Exception("This Task manager has not been implemented yet: " 
                    + manager_class);
  }

  // Base paramaters
  p_task_mngr->SetBaseParameters(m_img_ulx, m_img_uly, 
                                 m_img_width, m_img_height,
                                 m_tsk_width, m_tsk_height);

  // Additional parameters
  va_list(params);
  va_start(params, nb_params);
  for (int arg = 0; arg < nb_params; arg++)
    p_task_mngr->SetAdditionalParameters(arg, va_arg(params, int));
  va_end(params);
  
  // Create Task list
  p_task_mngr->CreateTaskList();
  //p_task_mngr->PrintTaskList();

  // Try to load the previous image
  bool prev_build = false;
  Camera* camera = p_scenery->getCamera(0);

  if (b_overwrite == false) {
  	ImageParser parser;
    prev_build = true;
    try {
      p_image = parser.load(camera->getOutputFilename() );
    } catch(Exception exc) {
      prev_build = false;
    }
  }

  if (prev_build == false) {
    p_image = new Image(camera->getWidth(), camera->getHeight(), 
                        camera->getNumberOfChannels());
    for(unsigned int i = 0; i < camera->getNumberOfChannels(); i++) {
      p_image->setChannelName(i, camera->getChannelName(i));
    }
    p_image->clear();
  }
}  
////////////////////////////// class StandAloneExecutor //////////////////////////
void StandAloneExecutor::Execute(void) {
  // Get the first camera point of view (We assume there is only one camera 
  // in the scene)
  Camera* camera = p_scenery->getCamera(0);
  // Redefine the image chunk in the particular case when the must be 
  // automatically computed
  if (m_chunk == -1) {
    m_chunk = p_task_mngr->nb_tasks() / m_nb_openmp_process;
  }
  // Prepare the Open loop by defining the number of OpenMP processes to be used
  omp_set_num_threads(m_nb_openmp_process);

  // Private variables for OpenMP : each process has its own value
  int i, ulx, uly, brx, bry;
  // Shared variables for OpenMP : the same value for all processes
  ImageParser parser;
  int nbtask = 0;

  // OpenMP Loop
# pragma omp parallel for private(i, ulx, uly, brx, bry) schedule(dynamic, m_chunk)
  for (i = 0; i < p_task_mngr->nb_tasks(); i++) {
    
    // Get the current task unit for the current OpenMP process
    p_task_mngr->GetTaskAt((unsigned int)i).Retrieve((unsigned int&)ulx, 
                                                     (unsigned int&)uly, 
                                                     (unsigned int&)brx, 
                                                     (unsigned int&)bry);
    // Compute this task unit
    camera->takeShot(*p_scenery, ulx, brx, uly, bry, *p_image);

		// Increment the visual counter in command console
    std::cout << "\rCamera " << 0 << " : " 
              << nbtask  * 100 / (p_task_mngr->nb_tasks() - 1) 
              << "%                                                           ";
    // Increment the number of task units that has been computed
    nbtask++;

    // Save the image (every m_nb_task_refresh task units)
    if (nbtask % m_nb_task_refresh == 0) {
      // This is the same image data, so must be a critical section
#     pragma omp critical
      parser.save(*p_image, camera->getOutputFilename());
    }
  
  } // end of the OpenMP loop, do a last saving operation to ensure all the 
  // image is saved
  parser.save(*p_image, camera->getOutputFilename());

//  // Private
//  int h, w;
//  LightVector ray;
//  Pixel pixel(camera->getNumberOfChannels());
//
//  for (i = 0; i < p_task_mngr->nb_tasks(); i++) {
//    p_task_mngr->GetTaskAt((unsigned int)i).Retrieve((unsigned int&)ulx, 
//                                                     (unsigned int&)uly, 
//                                                     (unsigned int&)brx, 
//                                                     (unsigned int&)bry);
//    VrtLog::Write("(%d, %d) - (%d, %d)", ulx, uly, brx, bry);
//#   pragma omp parallel for private(h, w, ray, pixel)
//    for (h = uly; h < bry; h++) {
//      for (w = ulx; w < brx; w++) {
//
//        VrtLog::Write("[%d] (%d, %d)", omp_get_thread_num(), w, h);
//
//        // Generate first ray from the pixel
//        camera->getRay(w, h, ray);
//
//        VrtLog::Write("[%d] get ray", omp_get_thread_num());
//
//        //Compute the light data
//        p_scenery->getRenderer()->CastRay(*p_scenery, ray);
//
//        //Project the light data into the color representation and update the image
//        camera->getColorHandler()->lightDataToRGB(ray, pixel);
//        image.setPixel(w, h, pixel);
//      }
//    }
//
//    //Save the image
//    parser.save(image, camera->getOutputFilename());
//
//		// Increment the visual counter
//    std::cout << "\rCamera " << 0 << " : " 
//              << nbtask  * 100 / p_task_mngr->nb_tasks() << "%        ";
//    nbtask++;
//  }
}
////////////////////////////////////////////////////////////////////////////////


