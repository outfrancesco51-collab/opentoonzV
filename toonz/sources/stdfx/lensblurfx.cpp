#include "stdfx.h"
#include "tfxparam.h"
#include "tparamset.h"

class LensBlurFx final : public TStandardRasterFx {
  FX_PLUGIN_DECLARATION(LensBlurFx)
  TRasterFxPort m_input;
  TDoubleParamP m_radius;
  TDoubleParamP m_irisShape;

public:
  LensBlurFx() : m_radius(5.0), m_irisShape(0.0) {
    addInputPort("Source", m_input);
    bindParam(this, "radius", m_radius);
    bindParam(this, "irisShape", m_irisShape);
    m_radius->setValueRange(0.0, 100.0);
    m_irisShape->setValueRange(0.0, 1.0);
  }
  ~LensBlurFx() {}

  bool doGetBBox(double frame, TRectD &bBox, const TRenderSettings &info) override {
    if (m_input.isConnected()) {
      bool ret = m_input->doGetBBox(frame, bBox, info);
      bBox = bBox.enlarge(m_radius->getValue(frame));
      return ret;
    }
    return false;
  }
  void doCompute(TTile &tile, double frame, const TRenderSettings &ri) override {
    if (!m_input.isConnected()) return;
    m_input->compute(tile, frame, ri);
  }
  bool canHandle(const TRenderSettings &info, double frame) override {
    return true;
  }
};
FX_PLUGIN_IDENTIFIER(LensBlurFx, "lensBlurFx")
