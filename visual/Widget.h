#ifndef WIDGET_H
#define WIDGET_H

#include "Common/Common.h"
#include "visual/Shader.h"

using namespace PBD;

class MeshWidget
{
    public:
        MeshWidget();
        void init(int argc, char **argv);

        void update();
        void render();
        void renderRigidBodys();

        void initShaders();
        void shaderBegin(const float *col);
		void shaderEnd();
        void shaderFlatBegin(const float* col);
        void shaderFlatEnd();

        const std::string& getExePath() const { return m_exePath; }

    protected:
        Shader m_shader;
		Shader m_shaderFlat;
		Shader m_shaderTex;

    private:
        std::string m_exePath;
};
#endif