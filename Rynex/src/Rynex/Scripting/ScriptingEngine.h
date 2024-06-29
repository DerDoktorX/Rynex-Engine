#pragma once


namespace Rynex {
	class ScriptingEngine
	{
	public:

		static void Init();
		static void Shutdown();
	private:
		static void InitMono();
		static void ShutDownMono();
	};
}


