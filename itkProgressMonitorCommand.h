#pragma once

#include <sstream>

#include <itkCommand.h>
#include <itkEventObject.h>
#include <itkProcessObject.h>

namespace itk
{

class ProgressMonitorCommand : public itk::Command
{
public:
  typedef ProgressMonitorCommand Self;
  typedef itk::Command Superclass;

  typedef itk::SmartPointer<Self> Pointer;

  itkNewMacro(Self);

  itkSetMacro(BarWidth, unsigned int);
  itkGetConstMacro(BarWidth, unsigned int);

protected:
  ProgressMonitorCommand()
    : m_BarWidth(50)
    , m_CallsCount(0)
  {
  }

  ~ProgressMonitorCommand() {}

  void Execute(itk::Object* caller, const itk::EventObject& event) override
  {
    Execute((const itk::Object*)caller, event);
  }

  void Execute(const itk::Object* caller, const itk::EventObject& event) override
  {
    ++m_CallsCount;

    float progress = 0.0;

    const itk::ProcessObject* procObj = dynamic_cast<const itk::ProcessObject*>(caller);

    progress = procObj->GetProgress();

    if (m_CallsCount == 1) {
      std::cout << std::endl;
    }

    std::cout << "[";

    unsigned int pos = m_BarWidth * progress;

    for (unsigned int i = 0; i < m_BarWidth; ++i) {
      if (i < pos)
        std::cout << "=";
      else if (i == pos)
        std::cout << ">";
      else
        std::cout << " ";
    }

    std::cout << "] " << int(progress * 100.0) << " %" << "\r";
    std::cout.flush();
}

protected:
  unsigned int m_BarWidth;
  size_t m_CallsCount;
};

} // end namespace itk
