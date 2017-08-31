# itkcvbf
2D/3D/4D CPU/GPU bilateral filter based on ITK and OpenCV

## Limitations
The filter only supports grayscale images.

## Usage
```cpp
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include "itkOpenCVBasedBilateralImageFilter.h"

const unsigned int Dimension = 3U;
using PixelType = signed short;
using ImageType = itk::Image<PixelType, Dimension>;

auto reader = itk::ImageFileReader<ImageType>::New();
reader->SetFileName("your_image_3d.nrrd")

auto bilateral = itk::OpenCVBasedBilateralImageFilter<ImageType>::New();
bilateral->SetRangeSigma(15);
bilateral->SetDomainSigma(5);
bilateral->SetInput(reader->GetOutput());

auto writer = itk::ImageFileWriter<ImageType>::New();
writer->SetFileName("your_image_3d_bf.nrrd")
writer->SetInput(bilateral->GetOutput());

try {
  writer->Update();
}
catch (itk::ExceptionObject & err) {
  std::cerr << err << std::endl;
  throw;
}
```

## Examples
| Brain CT axial slice | Result with rs=30, ds=4 | Second pass with rs=8, ds=2 (removing artifacts) |
| -------------------- | ----------------------- | ------------------------------------------------ |
| ![](https://user-images.githubusercontent.com/1299189/29918128-f3db4194-8e4c-11e7-9d25-f0712c45467c.png) | ![](https://user-images.githubusercontent.com/1299189/29918146-06de1168-8e4d-11e7-8ded-7121546fad03.png) | ![](https://user-images.githubusercontent.com/1299189/29918158-12cc7f8c-8e4d-11e7-8bbc-bb63d5e729fc.png) |
