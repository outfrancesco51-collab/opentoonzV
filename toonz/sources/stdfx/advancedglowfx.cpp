#include "stdfx.h"
#include "tfxparam.h"
#include "tparamset.h"

class AdvancedGlowFx final : public TStandardRasterFx {
  FX_PLUGIN_DECLARATION(AdvancedGlowFx)
  TRasterFxPort m_input;
  TDoubleParamP m_intensity;
  TDoubleParamP m_blur;

public:
  AdvancedGlowFx() : m_intensity(1.0), m_blur(5.0) {
    addInputPort("Source", m_input);
    bindParam(this, "intensity", m_intensity);
    bindParam(this, "blur", m_blur);
    m_intensity->setValueRange(0.0, 10.0);
    m_blur->setValueRange(0.0, 100.0);
  }
  ~AdvancedGlowFx() {}

  bool doGetBBox(double frame, TRectD &bBox, const TRenderSettings &info) override {
    if (m_input.isConnected()) {
      bool ret = m_input->doGetBBox(frame, bBox, info);
      bBox = bBox.enlarge(m_blur->getValue(frame));
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
FX_PLUGIN_IDENTIFIER(AdvancedGlowFx, "advancedGlowFx")
