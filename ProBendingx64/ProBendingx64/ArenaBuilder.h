#pragma once

class IScene;

class ArenaBuilder
{
public:
	///<summary>Creates the probending arena in code to be able to serialize required PhysX data</summary>
	///<param name="scene">The scene for objects to be added to</param>
	static void GenerateProbendingArena(IScene* scene);
};