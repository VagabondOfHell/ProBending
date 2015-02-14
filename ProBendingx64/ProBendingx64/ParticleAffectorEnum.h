#pragma once

namespace ParticleAffectorType
{
	//Bitshift to allow flags
	enum ParticleAffectorType{None = 0, 
		Scale = 1,
		AlphaFade = 1 << 1,
		ColourToColour = 1 << 2,
		Texture = 1 << 3
	};

	typedef unsigned short ParticleAffectorFlag;
};