#include "stdfx.h"
#include "tfxparam.h"
#include "tparamset.h"

class FilmGrainFx final : public TStandardRasterFx {
  FX_PLUGIN_DECLARATION(FilmGrainFx)
  TRasterFxPort m_input;
  TDoubleParamP m_intensity;
  TDoubleParamP m_size;

public:
  FilmGrainFx() : m_intensity(1.0), m_size(1.0) {
    addInputPort("Source", m_input);
    bindParam(this, "intensity", m_intensity);
    bindParam(this, "size", m_size);
    m_intensity->setValueRange(0.0, 10.0);
    m_size->setValueRange(0.1, 10.0);
  }
  ~FilmGrainFx() {}

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
FX_PLUGIN_IDENTIFIER(FilmGrainFx, "filmGrainFx")
