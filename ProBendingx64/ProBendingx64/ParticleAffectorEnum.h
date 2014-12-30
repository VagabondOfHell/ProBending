#pragma once

namespace ParticleAffectorType
{
	//Bitshift to allow flags
	enum ParticleAffectorType{None = 0, 
		Scale = 1,
		AlphaFade = 1 << 1,
		ColourToColour = 1 << 2
	};

	typedef char ParticleAffectorFlag;
};