#pragma once

#include <opencv2/core/cuda.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/cudaimgproc.hpp>

#include <itkProgressAccumulator.h>

#include "itkOpenCVBasedBilateralImageFilter.h"


namespace itk
{

inline void _OpenCVBasedBilateralImageFilter<2U>::GenerateData()
{
  this->UpdateProgress(0.0);
  this->AllocateOutputs();

  ImageType *input = const_cast<ImageType *>(this->GetInput());
  ImageType *output = this->GetOutput();

  auto sz = input->GetLargestPossibleRegion().GetSize();

  // OpenCV Mat as warpper over ITK images memory
  cv::Mat src(sz[0], sz[1], CV_32F, input->GetBufferPointer());
  cv::Mat dst(sz[0], sz[1], CV_32F, output->GetBufferPointer());

  if (cv::cuda::getCudaEnabledDeviceCount() > 0 && !this->m_CpuForce) {
    // GPU (CUDA) bilateral filtering
    cv::cuda::GpuMat gpuSrc;
    gpuSrc.upload(src);
    cv::cuda::GpuMat gpuDst(sz[0], sz[1], CV_32F);

    cv::cuda::bilateralFilter(gpuSrc, gpuDst, 0, this->m_RangeSigma, this->m_DomainSigma);
    
    if (this->m_Correction) {
      cv::cuda::bilateralFilter(gpuDst.clone(), gpuDst, 0, this->m_CorrectionRangeSigma, this->m_CorrectionDomainSigma);
    }

    gpuDst.download(dst);
  }
  else {
    cv::bilateralFilter(src, dst, 0, this->m_RangeSigma, this->m_DomainSigma);

    if (this->m_Correction) {
      cv::bilateralFilter(dst.clone(), dst, 0, this->m_CorrectionRangeSigma, this->m_CorrectionDomainSigma);
    }
  }

  this->UpdateProgress(1.0);
}

template <unsigned Dimension>
void _OpenCVBasedBilateralImageFilter<Dimension>::GenerateData()
{
  auto bilateral = BilateralImageFilter::New();
  bilateral->SetRangeSigma(this->m_RangeSigma);
  bilateral->SetDomainSigma(this->m_DomainSigma);
  bilateral->SetCorrection(this->m_Correction);
  bilateral->SetCorrectionRangeSigma(this->m_CorrectionRangeSigma);
  bilateral->SetCorrectionDomainSigma(this->m_CorrectionDomainSigma);
  bilateral->SetCpuForce(this->m_CpuForce);

  auto sliceBySlice = SliceBySliceImageFilter::New();
  sliceBySlice->SetInput(this->GetInput());
  sliceBySlice->SetFilter(bilateral);
  
  auto progress = itk::ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);
  progress->RegisterInternalFilter(sliceBySlice, 1.0);

  sliceBySlice->Update();

  this->GraftOutput(sliceBySlice->GetOutput());
}

template <typename TInputImage, typename TOutputImage>
void OpenCVBasedBilateralImageFilter<TInputImage, TOutputImage>::GenerateData()
{
  this->UpdateProgress(0.0);

  auto castToInternal = CastToInternalImageFilter::New();
  castToInternal->SetInput(this->GetInput());

  auto bilateral = InternalBilateralImageFilter::New();
  bilateral->SetInput(castToInternal->GetOutput());
  bilateral->SetRangeSigma(m_RangeSigma);
  bilateral->SetDomainSigma(m_DomainSigma);
  bilateral->SetCorrection(m_Correction);
  bilateral->SetCorrectionRangeSigma(m_CorrectionRangeSigma);
  bilateral->SetCorrectionDomainSigma(m_CorrectionDomainSigma);
  bilateral->SetCpuForce(m_CpuForce);

  auto progress = itk::ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);
  progress->RegisterInternalFilter(bilateral, 1.0);

  auto castFromInternal = CastFromInternalImageFilter::New();
  castFromInternal->SetInput(bilateral->GetOutput());
  castFromInternal->Update();

  this->GraftOutput(castFromInternal->GetOutput());
}

} // namespace itk
