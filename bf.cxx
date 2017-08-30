#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkTimeProbe.h>

#include "args.hxx"

#include "itkProgressMonitorCommand.h"
#include "itkOpenCVBasedBilateralImageFilter.h"


using ImageType = itk::Image<float, 4U>;



int main(int argc, char** argv)
{
  args::ArgumentParser parser("Bilateral filtering", "");

  args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});

  args::Group group(parser, "Required arguments:", args::Group::Validators::All);
  args::ValueFlag<std::string> argInput(group, "string", "Input image file name", {'i', "input"});
  args::ValueFlag<std::string> argOutput(group, "string", "Output image file name", {'o', "output"});
  
  args::ValueFlag<float> argRangeSigma(parser, "float", "Range sigma value", {'r', "range-sigma"}, 10.f);
  args::ValueFlag<float> argDomainSigma(parser, "float", "Domain sigma value", {'d', "domain-sigma"}, 5.f);

  args::Flag argCpuForce(parser, "cpu", "Forced using CPU", {'c', "cpu"});

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

  auto imageReader = itk::ImageFileReader<ImageType>::New();
  imageReader->SetFileName(argInput.Get());

  auto bilateral = itk::OpenCVBasedBilateralImageFilter<ImageType>::New();
  bilateral->SetInput(imageReader->GetOutput());
  bilateral->SetRangeSigma(argRangeSigma.Get());
  bilateral->SetDomainSigma(argDomainSigma.Get());
  bilateral->SetCpuForce(argCpuForce.Get());

  auto progmon = itk::ProgressMonitorCommand::New();
  bilateral->AddObserver(itk::ProgressEvent(), progmon);

  itk::TimeProbe tp;
  tp.Start();
  
  try {
    bilateral->Update();
  }
  catch (itk::ExceptionObject &err) {
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
  }
  
  tp.Stop();
  std::cout << "\nElapsed time: " << tp.GetTotal() << " sec" << std::endl;

  auto imageWriter = itk::ImageFileWriter<ImageType>::New();
  imageWriter->SetFileName(argOutput.Get());
  imageWriter->SetInput(bilateral->GetOutput());
  imageWriter->Update();

  return EXIT_SUCCESS;
}
