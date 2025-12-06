// DO NOT MARK.
//This is because it has been submitted for my dissertation. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Dissertation-SL295211.git


#pragma once
#include "BaseLight.h"

class SpotLight final : public BaseLight
{
public:

	//TODO make this a world object, not just move with the player, and dependent on its look direction
	/**
	 * A spotlight, think of a flashlight
	 * @param Ambient The amount of ambient light it gives off. Light that is always their
	 * @param Diffuse The spread of the light
	 * @param Specular How shiny is the shiny spot
	 * @param InShininess how large is the shiny spot
	 * @param InnerCutOff This is the soft cutoff, after this is starts to fade
	 * @param OuterCutOff This is the hard cutoff, no light after this
	 */
	SpotLight(const Vector3D& Ambient, const Vector3D& Diffuse, const Vector3D& Specular, float InShininess,
	          float InnerCutOff,
	          float OuterCutOff);

	void Implement(const Shader* InShader) override;

protected:

	float innerCutOff, outerCutOff;
};
