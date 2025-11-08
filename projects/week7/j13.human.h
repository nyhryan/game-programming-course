//////////////////////////////////////////
//		jieunlee@hansung.ac.kr			//
//		2020. 10. 12					//
//////////////////////////////////////////

#ifndef HUMAN_H
#define HUMAN_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "_shader.h"

#include <string>
#include <vector>

// Bones in a Human skeleton
enum Human_Bone
{ // 20 bones
    pelvis, spine, neck, head,
    // 쇄골
    clavicleL,upperarmL,forearmL,handL,
    clavicleR,upperarmR,forearmR,handR,
    thighL,calfL,footL,toeL,
    thighR,calfR,footR,toeR,
    BoneCount,
};

// several poses
enum Human_Pose
{
    base,
    armLeftUp, //
    walk_1,walk_2,walk_3,walk_4, //
    greet_0,greet_1,greet_2,greet_3,greet_4, //
    PoseCount
};

constexpr std::string Human_PoseToString(Human_Pose pose)
{
    switch (pose)
    {
    case base:
        return "base";
    case armLeftUp:
        return "armLeftUp";
    case walk_1:
        return "walk_1";
    case walk_2:
        return "walk_2";
    case walk_3:
        return "walk_3";
    case walk_4:
        return "walk_4";
    case greet_0:
        return "greet_0";
    case greet_1:
        return "greet_1";
    case greet_2:
        return "greet_2";
    case greet_3:
        return "greet_3";
    case greet_4:
        return "greet_4";
    default:
        return "unknown";
    }
}

// Default values
const int BONENUM = BoneCount;
const int POSENUM = PoseCount;

// An abstract camera class that processes input and calculates the corresponding Euler Angles,
// Vectors and Matrices for use in OpenGL
class Human
{
public:
    // Skeleton Attributes
    glm::quat BoneRotate[BONENUM];

    // Constructor
    Human()
    {
        // bone length
        BoneLength[pelvis] = 1.0f;
        BoneLength[spine] = 3.0f;
        BoneLength[neck] = 1.0f;
        BoneLength[head] = 1.0f;
        BoneLength[clavicleL] = 1.0f;
        BoneLength[upperarmL] = 2.0f;
        BoneLength[forearmL] = 1.5f;
        BoneLength[handL] = 1.0f;
        BoneLength[clavicleR] = 1.0f;
        BoneLength[upperarmR] = 2.0f;
        BoneLength[forearmR] = 1.5f;
        BoneLength[handR] = 1.0f;
        BoneLength[thighL] = 2.5f;
        BoneLength[calfL] = 2.0f;
        BoneLength[footL] = 1.0f;
        BoneLength[toeL] = 0.5f;
        BoneLength[thighR] = 2.5f;
        BoneLength[calfR] = 2.0f;
        BoneLength[footR] = 1.0f;
        BoneLength[toeR] = 0.5f;

        // setup poses
        SetupPoses();
        // bone rotation - base pose
        SetPose(base);
    }

    void SetPose(Human_Pose pose)
    {
        for (int i = pelvis; i < BoneCount; i++)
            BoneRotate[i] = Pose[pose][i];
    }

    void SetBoneRotation(Human_Bone bone, glm::quat q)
    {
        BoneRotate[bone] = q;
    }

    void CloneCurrentBoneRotation(std::vector<glm::quat> &out) const
    {
        out.clear();
        for (std::size_t i = pelvis; i < BoneCount; ++i)
        {
            out.push_back(BoneRotate[i]);
        }
    }

    void MixPose(Human_Pose p, Human_Pose q, float t)
    {
        for (int i = pelvis; i < BoneCount; i++)
            BoneRotate[i] = glm::slerp(Pose[p][i], Pose[q][i], t);
    }

    void MixPose(const std::vector<glm::quat> &boneRotations, Human_Pose to, float t)
    {
        for (int i = pelvis; i < BoneCount; i++)
            BoneRotate[i] = glm::slerp(boneRotations[i], Pose[to][i], t);
    }

    void DrawHuman(Shader shader, unsigned int cubeVAO, glm::mat4 model)
    {
        glm::mat4 bone = model;
        glm::mat4 mpelvis = model;
        glm::mat4 mspine;

        auto drawCall = [&](Human_Bone boneType, float r, float g, float b) -> void
        {
            shader.setMat4("model", bone);
            shader.setVec3("objectColor", r, g, b);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        };

        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        //  draw pelvis
        //  2. 모델 행렬을 곱해서 월드 변환 <- 1. 뼈들을 미리 회전변환하고
        //                     bone   *  glm::mat4_cast(...);
        bone = bone * glm::mat4_cast(BoneRotate[pelvis]);
        bone = glm::scale(bone, glm::vec3(1.0f, BoneLength[pelvis], 1.0f));
        drawCall(pelvis, 1.0f, 1.0f, 0.0f);
        bone = glm::scale(bone, glm::vec3(1.0f, 1.0f / BoneLength[pelvis], 1.0f));

        // draw spine
        bone = glm::translate(bone, glm::vec3(0.0f, BoneLength[pelvis], 0.0f));
        bone = bone * glm::mat4_cast(BoneRotate[spine]);
        bone = glm::scale(bone, glm::vec3(1.0f, BoneLength[spine], 1.0f));
        drawCall(spine, 1.0f, 0.5f, 0.3f);
        bone = glm::scale(bone, glm::vec3(1.0f, 1.0f / BoneLength[spine], 1.0f));
        mspine = bone;

        // draw neck
        bone = glm::translate(bone, glm::vec3(0.0f, BoneLength[spine], 0.0f));
        bone = bone * glm::mat4_cast(BoneRotate[neck]);
        bone = glm::scale(bone, glm::vec3(0.5f, BoneLength[neck], 0.5f));
        drawCall(neck, 0.0f, 0.5f, 1.0f);
        bone = glm::scale(bone, glm::vec3(2.0f, 1.0f / BoneLength[neck], 2.0f));

        // draw head
        bone = glm::translate(bone, glm::vec3(0.0f, BoneLength[neck], 0.0f));
        bone = bone * glm::mat4_cast(BoneRotate[head]);
        bone = glm::scale(bone, glm::vec3(1.0f, BoneLength[head], 1.0f));
        drawCall(head, 0.5f, 0.5f, 1.0f);
        bone = glm::scale(bone, glm::vec3(1.0f, 1.0f / BoneLength[head], 1.0f));

        // draw clavicleL
        bone = mspine;
        bone = glm::translate(bone, glm::vec3(0.5f, BoneLength[spine], 0.0f));
        bone = bone * glm::mat4_cast(BoneRotate[clavicleL]);
        bone = glm::scale(bone, glm::vec3(0.5f, BoneLength[clavicleL], 0.5f));
        drawCall(clavicleL, 0.0f, 0.7f, 0.0f);
        bone = glm::scale(bone, glm::vec3(2.0f, 1.0f / BoneLength[clavicleL], 2.0f));

        // draw upperarmL
        bone = mspine;
        bone = glm::translate(bone, glm::vec3(0.5f, BoneLength[spine], 0.0f));
        bone = glm::translate(bone, glm::vec3(BoneLength[clavicleL], 0.0f, 0.0f));
        bone = bone * glm::mat4_cast(BoneRotate[upperarmL]);
        bone = glm::scale(bone, glm::vec3(0.5f, BoneLength[upperarmL], 0.5f));
        drawCall(upperarmL, 0.3f, 0.0f, 0.7f);
        bone = glm::scale(bone, glm::vec3(2.0f, 1.0f / BoneLength[upperarmL], 2.0f));

        // draw forearmL
        bone = glm::translate(bone, glm::vec3(0.0f, BoneLength[upperarmL], 0.0f));
        bone = bone * glm::mat4_cast(BoneRotate[forearmL]);
        bone = glm::scale(bone, glm::vec3(0.5f, BoneLength[forearmL], 0.5f));
        drawCall(forearmL, 0.7f, 0.0f, 0.5f);
        bone = glm::scale(bone, glm::vec3(2.0f, 1.0f / BoneLength[forearmL], 2.0f));

        // draw handL
        bone = glm::translate(bone, glm::vec3(0.0f, BoneLength[forearmL], 0.0f));
        bone = bone * glm::mat4_cast(BoneRotate[handL]);
        bone = glm::scale(bone, glm::vec3(0.5f, BoneLength[handL], 0.5f));
        drawCall(handL, 0.0f, 0.5f, 0.5f);
        bone = glm::scale(bone, glm::vec3(2.0f, 1.0f / BoneLength[handL], 2.0f));

        // draw clavicleR
        bone = mspine;
        bone = glm::translate(bone, glm::vec3(-0.5f, BoneLength[spine], 0.0f));
        bone = bone * glm::mat4_cast(BoneRotate[clavicleR]);
        bone = glm::scale(bone, glm::vec3(0.5f, BoneLength[clavicleR], 0.5f));
        drawCall(clavicleR, 0.0f, 0.7f, 0.0f);
        bone = glm::scale(bone, glm::vec3(2.0f, 1.0f / BoneLength[clavicleR], 2.0f));

        // draw upperarmR
        bone = mspine;
        bone = glm::translate(bone, glm::vec3(-0.5f, BoneLength[spine], 0.0f));
        bone = glm::translate(bone, glm::vec3(-BoneLength[clavicleR], 0.0f, 0.0f));
        bone = bone * glm::mat4_cast(BoneRotate[upperarmR]);
        bone = glm::scale(bone, glm::vec3(0.5f, BoneLength[upperarmR], 0.5f));
        drawCall(upperarmR, 0.3f, 0.0f, 0.7f);
        bone = glm::scale(bone, glm::vec3(2.0f, 1.0f / BoneLength[upperarmR], 2.0f));

        // draw forearmR
        bone = glm::translate(bone, glm::vec3(0.0f, BoneLength[upperarmR], 0.0f));
        bone = bone * glm::mat4_cast(BoneRotate[forearmR]);
        bone = glm::scale(bone, glm::vec3(0.5f, BoneLength[forearmR], 0.5f));
        drawCall(forearmR, 0.7f, 0.0f, 0.5f);
        bone = glm::scale(bone, glm::vec3(2.0f, 1.0f / BoneLength[forearmR], 2.0f));

        // draw handR
        bone = glm::translate(bone, glm::vec3(0.0f, BoneLength[forearmR], 0.0f));
        bone = bone * glm::mat4_cast(BoneRotate[handR]);
        bone = glm::scale(bone, glm::vec3(0.5f, BoneLength[handR], 0.5f));
        drawCall(handR, 0.0f, 0.5f, 0.5f);
        bone = glm::scale(bone, glm::vec3(2.0f, 1.0f / BoneLength[handR], 2.0f));

        // ================================ 다리 ================================
        // draw thighL
        bone = mpelvis;
        bone = bone * glm::mat4_cast(BoneRotate[thighL]);
        bone = glm::translate(bone, glm::vec3(.5f, -BoneLength[thighL], 0.f));
        bone = glm::scale(bone, glm::vec3(.5f, BoneLength[thighL], .5f));
        drawCall(thighL, 0.5f, 0.5f, 0.5f);
        bone = glm::scale(bone, glm::vec3(2.0f, 1.0f / BoneLength[thighL], 2.0f));

        // draw calfL
        bone = bone * glm::mat4_cast(BoneRotate[calfL]);
        bone = glm::translate(bone, glm::vec3(0.f, -BoneLength[calfL], 0.0f));
        bone = glm::scale(bone, glm::vec3(0.5f, BoneLength[calfL], 0.5f));
        drawCall(calfL, 0.0f, 0.5f, 0.5f);
        bone = glm::scale(bone, glm::vec3(2.0f, 1.0f / BoneLength[calfL], 2.0f));

        // draw footL
        bone = bone * glm::mat4_cast(BoneRotate[footL]);
        bone = glm::translate(bone, glm::vec3(0.f, -BoneLength[footL], 0.f));
        bone = glm::scale(bone, glm::vec3(0.5f, BoneLength[footL], 0.5f));
        drawCall(footL, 0.5f, 0.0f, 0.5f);
        bone = glm::scale(bone, glm::vec3(2.0f, 1.0f / BoneLength[footL], 2.0f));

        // draw toeL
        bone = bone * glm::mat4_cast(BoneRotate[toeL]);
        bone = glm::translate(bone, glm::vec3(0.f, -BoneLength[toeL], 0.f));
        bone = glm::scale(bone, glm::vec3(0.5f, BoneLength[toeL], 0.5f));
        drawCall(toeL, 0.5f, 0.5f, 0.0f);
        bone = glm::scale(bone, glm::vec3(2.0f, 1.0f / BoneLength[toeL], 2.0f));

        // draw thighR
        bone = mpelvis;
        bone = bone * glm::mat4_cast(BoneRotate[thighR]);
        bone = glm::translate(bone, glm::vec3(-.5f, -BoneLength[thighR], 0.f));
        bone = glm::scale(bone, glm::vec3(0.5f, BoneLength[thighR], 0.5f));
        drawCall(thighR, 0.5f, 0.5f, 0.5f);
        bone = glm::scale(bone, glm::vec3(2.0f, 1.0f / BoneLength[thighR], 2.0f));

        // draw calfR
        bone = bone * glm::mat4_cast(BoneRotate[calfR]);
        bone = glm::translate(bone, glm::vec3(0.f, -BoneLength[calfR], 0.0f));
        bone = glm::scale(bone, glm::vec3(0.5f, BoneLength[calfR], 0.5f));
        drawCall(calfR, 0.0f, 0.5f, 0.5f);
        bone = glm::scale(bone, glm::vec3(2.0f, 1.0f / BoneLength[calfR], 2.0f));

        // draw footR
        bone = bone * glm::mat4_cast(BoneRotate[footR]);
        bone = glm::translate(bone, glm::vec3(0.f, -BoneLength[footR], 0.f));
        bone = glm::scale(bone, glm::vec3(0.5f, BoneLength[footR], 0.5f));
        drawCall(footR, 0.5f, 0.0f, 0.5f);
        bone = glm::scale(bone, glm::vec3(2.0f, 1.0f / BoneLength[footR], 2.0f));

        // draw toeR
        bone = bone * glm::mat4_cast(BoneRotate[toeR]);
        bone = glm::translate(bone, glm::vec3(0.f, -BoneLength[toeR], 0.f));
        bone = glm::scale(bone, glm::vec3(0.5f, BoneLength[toeR], 0.5f));
        drawCall(toeR, 0.5f, 0.5f, 0.0f);
        bone = glm::scale(bone, glm::vec3(2.0f, 1.0f / BoneLength[toeR], 2.0f));

        // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

private:
    float BoneLength[BONENUM];
    glm::quat Pose[POSENUM][BONENUM];

    void SetupPoses()
    {
        using glm::angleAxis;
        constexpr auto rad = glm::radians<float>;
        using glm::vec3;

        constexpr auto X = vec3(1.f, 0.f, 0.f);
        constexpr auto Y = vec3(0.f, 1.f, 0.f);
        constexpr auto Z = vec3(0.f, 0.f, 1.f);

        for (int p = 0; p < POSENUM; p++)
        {
            for (int i = 0; i < BONENUM; i++)
                Pose[p][i] = angleAxis(rad(0.f), vec3(0.f, 0.f, 1.f));

            switch (p)
            {
            case base:
                Pose[base][clavicleL] = angleAxis(rad(-90.f), vec3(0.f, 0.f, 1.f));
                Pose[base][clavicleR] = angleAxis(rad(90.f), vec3(0.f, 0.f, 1.f));
                break;
            case armLeftUp:
                Pose[armLeftUp][clavicleL] = angleAxis(rad(-90.f), vec3(0.f, 0.f, 1.f));
                Pose[armLeftUp][upperarmL] = angleAxis(rad(50.f), vec3(0.f, 0.f, 1.f));
                Pose[armLeftUp][forearmL] = angleAxis(rad(50.f), vec3(0.f, 0.f, 1.f));
                Pose[armLeftUp][clavicleR] = angleAxis(rad(90.f), vec3(0.f, 0.f, 1.f));
                break;
            case walk_1: // contact 1
            {
                auto p = Pose[walk_1];
                p[clavicleR] = angleAxis(rad(90.f), Z);
                p[clavicleL] = angleAxis(rad(-90.f), Z);

                // 오른팔
                p[upperarmR] = angleAxis(rad(180.f + 30.f), X);
                p[forearmR] = angleAxis(rad(-20.f), X);

                // 왼팔
                p[upperarmL] = angleAxis(rad(180.f - 30.f), X);
                p[forearmL] = angleAxis(rad(-20.f), X);

                // 오른다리
                p[thighR] = angleAxis(rad(-30.f), X);
                p[calfR] = angleAxis(rad(20.f), X);
                p[footR] = angleAxis(rad(-90.f), X);
                // 왼다리
                p[thighL] = angleAxis(rad(30.f), X);
                p[calfL] = angleAxis(rad(20.f), X);
                p[footL] = angleAxis(rad(-90.f), X);
            }
            break;
            case walk_2: // passing 1
            {

                auto p = Pose[walk_2];
                // p[spine] = angleAxis(rad(10.f), X);
                p[clavicleR] = angleAxis(rad(90.f), Z);
                p[clavicleL] = angleAxis(rad(-90.f), Z);
                // 오른팔
                p[upperarmR] = angleAxis(rad(180.f + 10.f), X);
                p[forearmR] = angleAxis(rad(-10.f), X);
                // 왼팔
                p[upperarmL] = angleAxis(rad(180.f - 10.f), X);
                p[forearmL] = angleAxis(rad(-10.f), X);
                // 오른다리
                p[thighR] = angleAxis(rad(-10.f), X);
                p[calfR] = angleAxis(rad(10.f), X);
                p[footR] = angleAxis(rad(-90.f), X);
                // 왼다리
                p[thighL] = angleAxis(rad(10.f), X);
                p[calfL] = angleAxis(rad(10.f), X);
                p[footL] = angleAxis(rad(-90.f), X);
            }
            break;
            case walk_3: // contact 2
            {
                auto p = Pose[walk_3];
                p[clavicleR] = angleAxis(rad(90.f), Z);
                p[clavicleL] = angleAxis(rad(-90.f), Z);
                // 오른팔
                p[upperarmR] = angleAxis(rad(180.f - 30.f), X);
                p[forearmR] = angleAxis(rad(-20.f), X);

                // 왼팔
                p[upperarmL] = angleAxis(rad(180.f + 30.f), X);
                p[forearmL] = angleAxis(rad(-20.f), X);

                // 오른다리
                p[thighR] = angleAxis(rad(30.f), X);
                p[calfR] = angleAxis(rad(20.f), X);
                p[footR] = angleAxis(rad(-90.f), X);
                // 왼다리
                p[thighL] = angleAxis(rad(-30.f), X);
                p[calfL] = angleAxis(rad(20.f), X);
                p[footL] = angleAxis(rad(-90.f), X);
            }
            break;
            case walk_4: // passing 2
            {
                auto p = Pose[walk_4];
                // p[spine] = angleAxis(rad(-10.f), X);
                p[clavicleR] = angleAxis(rad(90.f), Z);
                p[clavicleL] = angleAxis(rad(-90.f), Z);
                // 오른팔
                p[upperarmR] = angleAxis(rad(180.f - 10.f), X);
                p[forearmR] = angleAxis(rad(-10.f), X);
                // 왼팔
                p[upperarmL] = angleAxis(rad(180.f + 10.f), X);
                p[forearmL] = angleAxis(rad(-10.f), X);
                // 오른다리
                p[thighR] = angleAxis(rad(10.f), X);
                p[calfR] = angleAxis(rad(10.f), X);
                p[footR] = angleAxis(rad(-90.f), X);
                // 왼다리
                p[thighL] = angleAxis(rad(-10.f), X);
                p[calfL] = angleAxis(rad(10.f), X);
                p[footL] = angleAxis(rad(-90.f), X);
            }
            break;
            case greet_0:
            {
                auto p = Pose[greet_0];

                p[clavicleR] = angleAxis(rad(90.f), Z);
                p[clavicleL] = angleAxis(rad(-90.f), Z);
                p[footR] = angleAxis(rad(-90.f), X);
                p[footL] = angleAxis(rad(-90.f), X);

                // Right arm on waist (rotate counter-clockwise = positive Z)
                p[upperarmR] = angleAxis(rad(-30.f), Z) * angleAxis(rad(180.f), X); // 곱으로 두 회전을 하나로 표현가능
                p[forearmR] = angleAxis(rad(-90.f), Z);
                p[handR] = angleAxis(rad(-30.f), Z);

                // Left arm hanging naturally
                p[upperarmL] = angleAxis(rad(180.f), X);
            }
            break;

            case greet_1:
            {
                auto p = Pose[greet_1];
                p[clavicleR] = angleAxis(rad(90.f), Z);
                p[clavicleL] = angleAxis(rad(-90.f), Z);
                p[footR] = angleAxis(rad(-90.f), X);
                p[footL] = angleAxis(rad(-90.f), X);

                // Right arm on waist
                p[upperarmR] = angleAxis(rad(-30.f), Z) * angleAxis(rad(180.f), X);
                p[forearmR] = angleAxis(rad(-90.f), Z);
                p[handR] = angleAxis(rad(-30.f), Z);

                // Left arm raises up
                p[upperarmL] = angleAxis(rad(90.f), Z) * angleAxis(rad(180.f), X);
                p[forearmL] = angleAxis(rad(-30.f), Z);
            }
            break;

            case greet_2:
            {
                auto p = Pose[greet_2];
                p[clavicleR] = angleAxis(rad(90.f), Z);
                p[clavicleL] = angleAxis(rad(-90.f), Z);
                p[footR] = angleAxis(rad(-90.f), X);
                p[footL] = angleAxis(rad(-90.f), X);

                // Right arm on waist
                p[upperarmR] = angleAxis(rad(-30.f), Z) * angleAxis(rad(180.f), X);
                p[forearmR] = angleAxis(rad(-90.f), Z);
                p[handR] = angleAxis(rad(-30.f), Z);

                // Left arm waving down
                p[upperarmL] = angleAxis(rad(90.f), Z) * angleAxis(rad(180.f), X);
                p[forearmL] = angleAxis(rad(-60.f), Z);
                p[handL] = angleAxis(rad(-20.f), Z);
            }
            break;

            case greet_3:
            {
                auto p = Pose[greet_3];
                p[clavicleR] = angleAxis(rad(90.f), Z);
                p[clavicleL] = angleAxis(rad(-90.f), Z);
                p[footR] = angleAxis(rad(-90.f), X);
                p[footL] = angleAxis(rad(-90.f), X);

                // Right arm on waist
                p[upperarmR] = angleAxis(rad(-30.f), Z) * angleAxis(rad(180.f), X);
                p[forearmR] = angleAxis(rad(-90.f), Z);
                p[handR] = angleAxis(rad(-30.f), Z);

                // Left arm waving up
                p[upperarmL] = angleAxis(rad(90.f), Z) * angleAxis(rad(180.f), X);
                p[forearmL] = angleAxis(rad(0.f), Z);
                p[handL] = angleAxis(rad(20.f), Z);
            }
            break;

            case greet_4:
            {
                auto p = Pose[greet_4];
                p[clavicleR] = angleAxis(rad(90.f), Z);
                p[clavicleL] = angleAxis(rad(-90.f), Z);
                p[footR] = angleAxis(rad(-90.f), X);
                p[footL] = angleAxis(rad(-90.f), X);

                // Right arm on waist
                p[upperarmR] = angleAxis(rad(-30.f), Z) * angleAxis(rad(180.f), X);
                p[forearmR] = angleAxis(rad(-90.f), Z);
                p[handR] = angleAxis(rad(-30.f), Z);

                // Left arm middle position
                p[upperarmL] = angleAxis(rad(90.f), Z) * angleAxis(rad(180.f), X);
                p[forearmL] = angleAxis(rad(-30.f), Z);
                p[handL] = angleAxis(rad(0.f), Z);
            }
            break;
            default:
                break;
            };
        }
    }
};
#endif