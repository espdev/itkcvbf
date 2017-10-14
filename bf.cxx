#include <iostream>

#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkTimeProbe.h>

#include "args.hxx"

#include "itkProgressMonitorCommand.h"
#include "itkOpenCVBasedBilateralImageFilter.h"


struct Options
{
  const std::string& input; 
  const std::string& output;
  float rsigma; 
  float dsigma; 
  bool corr; 
  float crsigma;
  float cdsigma; 
  bool use_cpu;
};


template<unsigned int Dimension>
bool bilateral(Options opt)
{
  using ImageType = itk::Image<float, Dimension>;

  auto imageReader = itk::ImageFileReader<ImageType>::New();
  imageReader->SetFileName(opt.input);

  auto bilateral = itk::OpenCVBasedBilateralImageFilter<ImageType>::New();
  bilateral->SetInput(imageReader->GetOutput());
  bilateral->SetRangeSigma(opt.rsigma);
  bilateral->SetDomainSigma(opt.dsigma);
  bilateral->SetCorrection(opt.corr);
  bilateral->SetCorrectionRangeSigma(opt.crsigma);
  bilateral->SetCorrectionDomainSigma(opt.cdsigma);
  bilateral->SetCpuForce(opt.use_cpu);

  auto progmon = itk::ProgressMonitorCommand::New();
  bilateral->AddObserver(itk::ProgressEvent(), progmon);

  itk::TimeProbe tp;
  tp.Start();

  try {
    bilateral->Update();
  }
  catch (itk::ExceptionObject &err) {
    std::cerr << err << std::endl;
    return false;
  }

  tp.Stop();
  std::cout << "\nElapsed time: " << tp.GetTotal() << " sec" << std::endl;

  auto imageWriter = itk::ImageFileWriter<ImageType>::New();
  imageWriter->SetFileName(opt.output);
  imageWriter->SetInput(bilateral->GetOutput());
  imageWriter->Update();

  return true;
}


int main(int argc, char** argv)
{
  args::ArgumentParser parser("Bilateral filtering", "");

  args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});

  args::Group group(parser, "Required arguments:", args::Group::Validators::All);
  args::ValueFlag<std::string> argInput(group, "INPUT", "Input image file name", {'i', "input"});
  args::ValueFlag<std::string> argOutput(group, "OUTPUT", "Output image file name", {'o', "output"});

  args::ValueFlag<unsigned int> argDimension(parser, "DIM", "Image dimension 2/[3]/4", {'d', "dimension"}, 3U);
  
  args::ValueFlag<float> argRangeSigma(parser, "RANGE_SIGMA", "Range sigma value", {'r', "range-sigma"}, 10.f);
  args::ValueFlag<float> argDomainSigma(parser, "DOMAIN_SIGMA", "Domain sigma value", {'d', "domain-sigma"}, 5.f);

  args::Flag argCorrecion(parser, "CORRECTION", "Artifacts correction", {'C', "correction"});
  args::ValueFlag<float> argCorrectionRangeSigma(parser, "CORR_RANGE_SIGMA", "Correction range sigma value", {'R', "corr-range-sigma"}, 8.f);
  args::ValueFlag<float> argCorrectionDomainSigma(parser, "CORR_DOMAIN_SIGMA", "Correction domain sigma value", {'D', "corr-domain-sigma"}, 2.f);

  args::Flag argCpuForce(parser, "CPU_FORCE", "Forced using CPU", {'c', "cpu"});

  try {
    parser.ParseCLI(argc, argv);
  }
  catch (args::Help) {
    std::cout << parser;
    return EXIT_SUCCESS;
  }
  catch (args::ParseError &e) {
    std::cerr << e.what() << std::endl;
    std::cerr << parser;
    return EXIT_FAILURE;
  }
  catch (args::ValidationError &e) {
    std::cerr << e.what() << std::endl;
    std::cerr << parser;
    return EXIT_FAILURE;
  }

  Options options = {
    argInput.Get(),
    argOutput.Get(),
    argRangeSigma.Get(),
    argDomainSigma.Get(),
    argCorrecion,
    argCorrectionRangeSigma.Get(),
    argCorrectionDomainSigma.Get(),
    argCpuForce
  };

  bool result = false;

  switch (argDimension.Get())
  {
  case 2:
    result = bilateral<2>(options);
    break;
  case 3:
    result = bilateral<3>(options);
    break;
  case 4:
    result = bilateral<4>(options);
    break;
  default:
    std::cerr << "Invalid image dimension " << argDimension.Get() << std::endl;
    result = false;
  }

  if (!result) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
