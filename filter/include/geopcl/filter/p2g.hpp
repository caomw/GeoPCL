#ifndef INCLUDED_POINTS_TO_GRID_HPP
#define INCLUDED_POINTS_TO_GRID_HPP

//#include <fstream>
#include <iostream>
#include <limits>
//#include <string>

//#include <liblas/liblas.hpp>

// may also need CoreInterp, export, ...
#include <points2grid/InCoreInterp.hpp>
#include <points2grid/OutCoreInterp.hpp>

#include <pcl/common/common.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
//#include <pcl/point_traits.h>

namespace geopcl
{
  /**
   * \brief Rasterize point cloud.
   *
   * Rasterize point cloud.
   */
  template <typename CloudT>
  void PointsToGrid(const CloudT &cloud, float resolution, CloudT &grid)
  {
    std::cout << resolution << " meters" << std::endl;

    /*
     * Compute extents of points.
     */
    typename CloudT::PointType min_pt, max_pt; //testPoint = cloud.points[0];
    pcl::getMinMax3D(cloud, min_pt, max_pt);
    std::cout << min_pt << std::endl;
    std::cout << max_pt << std::endl;

    /*
     * Compute size of grid.
     */
    boost::int32_t xsize = std::ceil((max_pt.x - min_pt.x) / resolution) + 1;
    boost::int32_t ysize = std::ceil((max_pt.y - min_pt.y) / resolution) + 1;
    std::cout << xsize << " x bins" << std::endl;
    std::cout << ysize << " y bins" << std::endl;

    // auto?
    if(xsize * ysize > MEM_LIMIT)
    {
      interpolation_mode = INTERP_OUTCORE;
    }
    else
    {
      interpolation_mode = INTERP_INCORE;
    }

    if(interpolation_mode == INTERP_OUTCORE)
    {
      std::cout << "Using out of core interp code" << std::endl;

      interp = new OutCoreInterp(xsize, ysize, xsize, ysize, radius_sqr,
        min_pt.x, max_pt.x, min_pt.x, max_pt.y, window_size);

      if(interp==NULL)
      {
        std::cout << "OutCoreInterp construction error" << std::endl;
        return;
      }
    }
    else
    {
      std::cout << "Using in core interp code" << std::endl;

      interp = new InCoreInter(xsize, ysize, xsize, ysize, radius_sqr,
        min_pt.x, max_pt.x, min_pt.y, max_pt.y, window_size);
    }

    if(interp->init() < 0)
    {
      std::cout << "interp->init() error" << std::endl;
      return;
    }

    std::cout << "Interpolation::init() done successfully" << std::endl;

    std::cout << "Interp starts" << std::endl;

    for (size_t idx = 0; idx < cloud.points.size(); ++idx)
    {
      typename CloudT::PointType p = cloud.points[idx];

      p.x -= min_pt.x;
      p.y -= min_pt.y;

      if((rc = interp->update(p.x, p.y, p.z)) < 0)
      {
        std::cout << "interp->update() error while processing" << std::endl;
        return;
      }
    }

    if ((rc = interp->finish(outputName, outputFormat, outputType)) < 0)
    {
      std::cout << "interp->finish() error" << std::endl;
      return;
    }
  }
}  // geopcl

#endif  // INCLUDED_POINTS_TO_GRID_HPP
