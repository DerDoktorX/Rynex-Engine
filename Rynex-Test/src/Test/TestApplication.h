#include <Rynex/Core/Application.h>

class TestApplication : public Rynex::Application
{
public:
    explicit TestApplication(const Rynex::ApplicationSpecification& spec)
        : Rynex::Application(spec)
    {}

    void RunShutdown()
    {
        this->Close();
    }

    ~TestApplication()
    {
    }

};
