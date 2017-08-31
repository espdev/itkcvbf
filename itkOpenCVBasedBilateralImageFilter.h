#pragma once

#include <itkImageToImageFilter.h>
#include <itkSliceBySliceImageFilter.h>
#include <itkCastImageFilter.h>


namespace itk
{

template <unsigned int Dimension>
class _OpenCVBasedBilateralImageFilterBase :
  public itk::ImageToImageFilter<itk::Image<float, Dimension>, 
                                 itk::Image<float, Dimension>>
{
public:
  itkSetMacro(RangeSigma, float);
  itkSetMacro(DomainSigma, float);
  itkSetMacro(Correction, bool);
  itkSetMacro(CorrectionRangeSigma, float);
  itkSetMacro(CorrectionDomainSigma, float);
  itkSetMacro(CpuForce, bool);

protected:
  float m_RangeSigma = 0.0;
  float m_DomainSigma = 0.0;
  bool m_Correction = false;
  float m_CorrectionRangeSigma = 0.0;
  float m_CorrectionDomainSigma = 0.0;
  bool m_CpuForce = false;
};

template <unsigned int Dimension>
class _OpenCVBasedBilateralImageFilter :
  public _OpenCVBasedBilateralImageFilterBase<Dimension>
{
public:
  using Self = _OpenCVBasedBilateralImageFilter;
  using Pointer = itk::SmartPointer<Self>;

  itkNewMacro(Self);

protected:
  using ImageType = itk::Image<float, Dimension>;
  using SliceBySliceImageFilter = itk::SliceBySliceImageFilter<ImageType, ImageType>;
  using BilateralImageFilter = _OpenCVBasedBilateralImageFilter<Dimension-1>;

  void GenerateData() override;
};

// Specialization for 2D bilateral filter
template <>
class _OpenCVBasedBilateralImageFilter<2U> :
  public _OpenCVBasedBilateralImageFilterBase<2U>
{
public:
  using Self = _OpenCVBasedBilateralImageFilter;
  using Pointer = itk::SmartPointer<Self>;

  itkNewMacro(Self);

protected:
  using ImageType = itk::Image<float, 2U>;

  void GenerateData() override;
};

template <typename TInputImage, typename TOutputImage = TInputImage>
class OpenCVBasedBilateralImageFilter : 
  public itk::ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  using Self = OpenCVBasedBilateralImageFilter;
  using Superclass = itk::ImageToImageFilter<TInputImage, TOutputImage>;

  using Pointer = itk::SmartPointer<Self>;
  using ConstPointer = itk::SmartPointer<const Self>;

  itkNewMacro(Self);
  itkTypeMacro(OpenCVBasedBilateralImageFilter, itk::ImageToImageFilter);

public:
  static_assert(TInputImage::ImageDimension == TOutputImage::ImageDimension,
    "Dimension of input and output images must be equal.");

  itkStaticConstMacro(ImageDimension, unsigned int, TInputImage::ImageDimension);

  static_assert(ImageDimension == 2U || ImageDimension == 3U || ImageDimension == 4U,
    "Only 2D, 3D and 4D images are supported.");

  using InputImageType = TInputImage;
  using OutputImageType = TOutputImage;

  itkSetMacro(RangeSigma, float);
  itkGetMacro(RangeSigma, float);

  itkSetMacro(DomainSigma, float);
  itkGetMacro(DomainSigma, float);

  itkSetMacro(Correction, bool);
  itkGetMacro(Correction, bool);
  itkBooleanMacro(Correction);

  itkSetMacro(CorrectionRangeSigma, float);
  itkGetMacro(CorrectionRangeSigma, float);

  itkSetMacro(CorrectionDomainSigma, float);
  itkGetMacro(CorrectionDomainSigma, float);

  itkSetMacro(CpuForce, bool);
  itkGetMacro(CpuForce, bool);
  itkBooleanMacro(CpuForce);

protected:
  using InternalImageType = itk::Image<float, ImageDimension>;

  using CastToInternalImageFilter = itk::CastImageFilter<InputImageType, InternalImageType>;
  using CastFromInternalImageFilter = itk::CastImageFilter<InternalImageType, OutputImageType>;
  using InternalBilateralImageFilter = _OpenCVBasedBilateralImageFilter<ImageDimension>;

protected:
  OpenCVBasedBilateralImageFilter() {}
  ~OpenCVBasedBilateralImageFilter() {}

  void GenerateData() override;

  float m_RangeSigma = 10.0;
  float m_DomainSigma = 5.0;

  bool m_Correction = false;
  float m_CorrectionRangeSigma = 8;
  float m_CorrectionDomainSigma = 2;

  bool m_CpuForce = false;

private:
  OpenCVBasedBilateralImageFilter(const Self &) = delete;
  void operator=(const Self &) = delete;
};

} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkOpenCVBasedBilateralImageFilter.hxx"
#endif
