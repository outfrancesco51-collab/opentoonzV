#pragma once

#ifndef ROTOSCOPY_FX_H
#define ROTOSCOPY_FX_H

#include "stdfx.h"
#include "tfxparam.h"
#include <opencv2/opencv.hpp>

class RotoscopyFx : public TStandardRasterFx {
  FX_PLUGIN_DECLARATION(RotoscopyFx)

protected:
  TRasterFxPort m_source;
  TDoubleParamP m_threshold1;
  TDoubleParamP m_threshold2;
  TDoubleParamP m_apertureSize;
  TBoolParamP m_L2gradient;

  template <typename RASTER, typename PIXEL>
  void applyCanny(const RASTER srcRas, RASTER dstRas, double t1, double t2, int aperture, bool l2);

public:
  RotoscopyFx();
  ~RotoscopyFx() override {}
  
  void doCompute(TTile &tile, double frame,
                 const TRenderSettings &settings) override;
                 
  bool doGetBBox(double frame, TRectD &bBox,
                 const TRenderSettings &info) override;
                 
  bool canHandle(const TRenderSettings &info, double frame) override;
};

#endif
