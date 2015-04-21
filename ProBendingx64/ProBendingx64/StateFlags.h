namespace StateFlags
{
	enum PossibleStates
	{
		INVALID_STATE,
		IDLE_STATE,
		JUMP_STATE,
		FALLING_STATE,
		BLOCK_STATE,
		CATCH_STATE,
		DODGE_STATE,
		REELING_STATE,
		ATTACKING_STATE,
		TRANSITION_STATE,
		COUNT
	};

	enum PossibleStateFlags
	{
		INVALID_STATE_FLAG = 0,
		IDLE_STATE_FLAG = 1 << 0,
		JUMP_STATE_FLAG = 1 << 1,
		FALLING_STATE_FLAG = 1 << 2,
		BLOCK_STATE_FLAG = 1 << 3,
		CATCH_STATE_FLAG = 1 << 4,
		DODGE_STATE_FLAG = 1 << 6,
		REELING_STATE_FLAG = 1 << 7,
		ATTACKING_STATE_FLAG = 1 << 8,
		TRANSITION_STATE_FLAG = 1 << 9,
		ALL_STATES = IDLE_STATE_FLAG | JUMP_STATE_FLAG | FALLING_STATE_FLAG | BLOCK_STATE_FLAG | 
						CATCH_STATE_FLAG | DODGE_STATE_FLAG | REELING_STATE_FLAG |
						ATTACKING_STATE_FLAG | TRANSITION_STATE_FLAG
	};

	typedef unsigned int StateFlagCombo;

	///<summary>Converts the enum value to a flag value</summary>
	///<param name="stateToConvert">The enum value to convert</param>
	///<returns>The flag value represented by the enum, or INVALID_FLAG if value not found or was equal to count</returns>
	static inline PossibleStateFlags ConvertToFlag(PossibleStates stateToConvert)
	{
		//Get flag value of enum
		switch (stateToConvert)
		{
		case StateFlags::IDLE_STATE:
			return StateFlags::IDLE_STATE_FLAG;
			break;
		case StateFlags::JUMP_STATE:
			return StateFlags::JUMP_STATE_FLAG;
			break;
		case StateFlags::FALLING_STATE:
			return StateFlags::FALLING_STATE_FLAG;
			break;
		case StateFlags::BLOCK_STATE:
			return StateFlags::BLOCK_STATE_FLAG;
			break;
		case StateFlags::CATCH_STATE:
			return StateFlags::CATCH_STATE_FLAG;
			break;
		case StateFlags::DODGE_STATE:
			return StateFlags::DODGE_STATE_FLAG;
			break;
		case StateFlags::REELING_STATE:
			return StateFlags::REELING_STATE_FLAG;
			break;
		case StateFlags::ATTACKING_STATE:
			return StateFlags::ATTACKING_STATE_FLAG;
			break;
		case StateFlags::TRANSITION_STATE:
			return StateFlags::TRANSITION_STATE_FLAG;
			break;
		default://if count or invalid, return invalid
			return StateFlags::INVALID_STATE_FLAG;
			break;
		}
	}
};