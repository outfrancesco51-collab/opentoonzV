#include "stdfx.h"
#include "tfxparam.h"
#include "tparamset.h"

class ColorGradingFx final : public TStandardRasterFx {
  FX_PLUGIN_DECLARATION(ColorGradingFx)
  TRasterFxPort m_input;
  TDoubleParamP m_contrast;
  TDoubleParamP m_brightness;
  TDoubleParamP m_saturation;

public:
  ColorGradingFx() : m_contrast(1.0), m_brightness(0.0), m_saturation(1.0) {
    addInputPort("Source", m_input);
    bindParam(this, "contrast", m_contrast);
    bindParam(this, "brightness", m_brightness);
    bindParam(this, "saturation", m_saturation);
    m_contrast->setValueRange(0.0, 5.0);
    m_brightness->setValueRange(-100.0, 100.0);
    m_saturation->setValueRange(0.0, 5.0);
  }
  ~ColorGradingFx() {}

  bool doGetBBox(double frame, TRectD &bBox, const TRenderSettings &info) override {
    if (m_input.isConnected()) {
      return m_input->doGetBBox(frame, bBox, info);
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
FX_PLUGIN_IDENTIFIER(ColorGradingFx, "colorGradingFx")
