#include "rotoscopyfx.h"
#include "tpixelutils.h"

RotoscopyFx::RotoscopyFx()
    : m_threshold1(100.0)
    : m_threshold2(200.0)
    : m_apertureSize(3.0)
    : m_L2gradient(false)
{
  addInputPort("Source", m_source);
  bindParam(this, "threshold1", m_threshold1);
  bindParam(this, "threshold2", m_threshold2);
  bindParam(this, "apertureSize", m_apertureSize);
  bindParam(this, "L2gradient", m_L2gradient);

  m_threshold1->setValueRange(0.0, 1000.0);
  m_threshold2->setValueRange(0.0, 1000.0);
  m_apertureSize->setValueRange(3.0, 7.0); // Aperture size must be 3, 5, or 7
}

template <typename RASTER, typename PIXEL>
void RotoscopyFx::applyCanny(const RASTER srcRas, RASTER dstRas, double t1, double t2, int aperture, bool l2) {
  int lx = srcRas->getLx();
  int ly = srcRas->getLy();

  cv::Mat srcMat(ly, lx, CV_8UC4);
  for (int y = 0; y < ly; ++y) {
    PIXEL *srcPix = srcRas->pixels(y);
    cv::Vec4b *matPix = srcMat.ptr<cv::Vec4b>(y);
    for (int x = 0; x < lx; ++x) {
      matPix[x][0] = srcPix[x].b;
      matPix[x][1] = srcPix[x].g;
      matPix[x][2] = srcPix[x].r;
      matPix[x][3] = srcPix[x].m;
    }
  }

  cv::Mat grayMat;
  cv::cvtColor(srcMat, grayMat, cv::COLOR_BGRA2GRAY);

  cv::Mat edges;
  cv::Canny(grayMat, edges, t1, t2, aperture, l2);

  for (int y = 0; y < ly; ++y) {
    PIXEL *dstPix = dstRas->pixels(y);
    uchar *edgePix = edges.ptr<uchar>(y);
    for (int x = 0; x < lx; ++x) {
      if (edgePix[x] > 128) {
        dstPix[x] = PIXEL::Black;
      } else {
        dstPix[x] = PIXEL::Transparent;
      }
    }
  }
}

void RotoscopyFx::doCompute(TTile &tile, double frame, const TRenderSettings &settings) {
  if (!m_source.isConnected()) {
    tile.getRaster()->clear();
    return;
  }

  m_source->compute(tile, frame, settings);

  TRaster32P ras32 = tile.getRaster();
  TRaster64P ras64 = tile.getRaster();

  double t1 = m_threshold1->getValue(frame);
  double t2 = m_threshold2->getValue(frame);
  int aperture = (int)m_apertureSize->getValue(frame);
  if (aperture % 2 == 0) aperture++; // Must be odd
  if (aperture < 3) aperture = 3;
  if (aperture > 7) aperture = 7;
  bool l2 = m_L2gradient->getValue();

  if (ras32) {
    applyCanny<TRaster32P, TPixel32>(ras32, ras32, t1, t2, aperture, l2);
  } else if (ras64) {
    applyCanny<TRaster64P, TPixel64>(ras64, ras64, t1, t2, aperture, l2);
  }
}

bool RotoscopyFx::doGetBBox(double frame, TRectD &bBox, const TRenderSettings &info) {
  if (m_source.isConnected()) {
    return m_source->doGetBBox(frame, bBox, info);
  }
  return false;
}

bool RotoscopyFx::canHandle(const TRenderSettings &info, double frame) {
  return true;
}

FX_PLUGIN_IDENTIFIER(RotoscopyFx, "rotoscopyFx")
