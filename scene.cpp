#include "scene.h"
#include "loader.h"
#include <QCoreApplication>
#include <QTime>

Scene::Scene(QWidget *parent) :
    QOpenGLWidget(parent){
    Loader::loadModel(QString(QCoreApplication::applicationDirPath()+"/Models/sphere.obj"), &vertices,&textureCord, &normals);
    texture = new QOpenGLTexture( QImage( QCoreApplication::applicationDirPath()+"/Textures/Earth.jpg" ).mirrored() );
    hRot = 0.0f;
    connect( &timer, SIGNAL( timeout() ), this, SLOT(update()));
    timer.setInterval(0);
    timer.start();
}

Scene::~Scene(){
    makeCurrent();
    delete texture;
    texture = NULL;
    doneCurrent();
}

void Scene::initializeGL(){
    //Создание вершинного шейдера
        QOpenGLShader vShader(QOpenGLShader::Vertex);
        vShader.compileSourceFile(":/Shaders/vShader.glsl");
    //Создание фрагментног2о шейдера
        QOpenGLShader fShader(QOpenGLShader::Fragment);
        fShader.compileSourceFile(":/Shaders/fShader.glsl");
    //Добавление шейдеров в конвеер и сборка ш. программы
        program.addShader(&vShader);
        program.addShader(&fShader);
        if ( !program.link() )
        {
            qWarning( "Error: unable to link a shader program." );
            return;
        }
    //Привязка атрибутов
        vertexAttr = program.attributeLocation( "vertexAttr" );
        textureAttr = program.attributeLocation("textureAttr");
        normalAttr = program.attributeLocation("normalAttr");

        textureUniform = program.uniformLocation("textureUniform");
        cameraPosUniform = program.uniformLocation("cameraPos");

        directionUniform = program.uniformLocation( "directionalLight.direction");
        directionColorUniform = program.uniformLocation("directionalLight.color");
        directionAmbientUniform = program.uniformLocation("directionalLight.ambientIntensity");
        directionDiffuseUniform = program.uniformLocation("directionalLight.diffuseIntensity");
        directionSpecularUniform = program.uniformLocation("directionalLight.specularIntensity");

        materialSpecularFactorUniform = program.uniformLocation( "materialSpecularFactor");
        materialEmissionUniform = program.uniformLocation( "materialEmission");

        mvpUniform = program.uniformLocation( "MVP" );
        mUniform = program.uniformLocation("M");


    //Начальные настройки OpenGL
        glClearColor(0.0f,0.0f,0.0f,1.0f);
    //Включение теста глубины
        glEnable(GL_DEPTH_TEST);
    //Включение отсечения невидимых граней
        glPolygonMode(GL_FRONT_AND_BACK, GL_POLYGON);
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable( GL_ALPHA_TEST );
        glEnable(GL_MULTISAMPLE);
}

void Scene::paintGL(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if ( !program.bind() ){
        qWarning("Cant bind shader program");
        return;
    }
    texture->bind();

    mMatrix.setToIdentity();
    mMatrix.translate( 0.0f, 0.0f, -2.0f );
    mMatrix.rotate( hRot, 0.0f, 1.0f, 0.0f );

    vMatrix.setToIdentity();
    vMatrix.lookAt( QVector3D( 0.0f , 5.0f, 5.0f), QVector3D( 0.0f, 0.0f, -2.0f ), QVector3D( 0, 1, 0 ) );

    program.setUniformValue( cameraPosUniform, QVector3D( 0.0f , 5.0f, 5.0f) );
    program.setUniformValue( mvpUniform, pMatrix * vMatrix * mMatrix );
    program.setUniformValue( mUniform, mMatrix );

    QVector3D vec( 1.0f, 0.0f, -1.0f );
    vec.normalize();
    program.setUniformValue( directionUniform, vec );
    program.setUniformValue( directionColorUniform, QVector3D( 1.0f, 1.0f, 1.0f ) );
    program.setUniformValue( directionAmbientUniform, 0.05f );
    program.setUniformValue( directionDiffuseUniform, 0.5f );
    program.setUniformValue( directionSpecularUniform, 0.1f );

    program.setUniformValue( materialSpecularFactorUniform, 1.0f );
    program.setUniformValue( materialEmissionUniform, 0.0f, 0.0f, 0.0f );

    program.setUniformValue( textureUniform, 0 );

    program.setAttributeArray( vertexAttr, vertices.data(), 3 );
    program.setAttributeArray( textureAttr, textureCord.data(), 2 );

    program.setAttributeArray( normalAttr, normals.data(), 3 );

    program.enableAttributeArray( vertexAttr );
    program.enableAttributeArray( textureAttr );
    program.enableAttributeArray( normalAttr );

    glDrawArrays( GL_TRIANGLES, 0, vertices.size() / 3 );

    program.disableAttributeArray( vertexAttr );
    program.disableAttributeArray( textureAttr );
    program.disableAttributeArray( normalAttr );
    texture->release();
    program.release();
    hRot = hRot >= 360.0f ? 0.0f : hRot+=0.3f;
}

void Scene::resizeGL(int w, int h){
    //Матрица проекции, использует: угол обзора, соотношение сторон и ближнюю и дальнюю стенки проекции
        float aspect = float(w) / float(h ? h : 1);
        const float zNear = 0.1f, zFar = 50.0f, fov = 60.0;
        pMatrix.setToIdentity();
        pMatrix.perspective(fov, aspect, zNear, zFar);
}
