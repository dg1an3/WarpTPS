#include "pch.h"
#include "CppUnitTest.h"
#include "WarpTpsLib.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest1
{
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
		}

		TEST_METHOD(TestAddLandmark)
		{
			Logger::WriteMessage("TestAddLandmark");

			auto tpsTransform = new CTPSTransform();			
			tpsTransform->AddLandmark(CVectorD<3>(0.0, 0.0, 0.0));
			tpsTransform->AddLandmark(CVectorD<3>(1.0, 0.0, 0.0));
			tpsTransform->AddLandmark(CVectorD<3>(0.0, 1.0, 0.0));
			tpsTransform->AddLandmark(CVectorD<3>(0.0, 0.0, 1.0));

			Assert::IsTrue(tpsTransform->GetLandmark(0, 0).IsApproxEqual(CVectorD<3>(0.0, 0.0, 0.0)));
			Assert::IsTrue(tpsTransform->GetLandmark(0, 1).IsApproxEqual(CVectorD<3>(1.0, 0.0, 0.0)));
			Assert::IsTrue(tpsTransform->GetLandmark(0, 2).IsApproxEqual(CVectorD<3>(0.0, 1.0, 0.0)));
			Assert::IsTrue(tpsTransform->GetLandmark(0, 3).IsApproxEqual(CVectorD<3>(0.0, 0.0, 1.0)));

			Assert::IsTrue(tpsTransform->GetLandmark(1, 0).IsApproxEqual(CVectorD<3>(0.0, 0.0, 0.0)));
			Assert::IsTrue(tpsTransform->GetLandmark(1, 1).IsApproxEqual(CVectorD<3>(1.0, 0.0, 0.0)));
			Assert::IsTrue(tpsTransform->GetLandmark(1, 2).IsApproxEqual(CVectorD<3>(0.0, 1.0, 0.0)));
			Assert::IsTrue(tpsTransform->GetLandmark(1, 3).IsApproxEqual(CVectorD<3>(0.0, 0.0, 1.0)));

			Logger::WriteMessage("Done TestAddLandmark");
		}

		TEST_METHOD(TestCalculateWarp)
		{
			Logger::WriteMessage("TestAddLandmark");

			auto tpsTransform = new CTPSTransform();
			tpsTransform->AddLandmark(CVectorD<3>(0.0, 0.0, 0.0));
			tpsTransform->AddLandmark(CVectorD<3>(1.0, 0.0, 0.0));
			tpsTransform->AddLandmark(CVectorD<3>(0.0, 1.0, 0.0));
			tpsTransform->AddLandmark(CVectorD<3>(0.0, 0.0, 1.0));

			CVectorD<3> vOriginal(0.0, 0.0, 0.0);
			CVectorD<3> vTransformed;
			tpsTransform->Eval(vOriginal, vTransformed, 0.0);
			Assert::IsTrue(vOriginal.IsApproxEqual(vTransformed));

			Logger::WriteMessage("Done TestAddLandmark");
		}

		TEST_METHOD(TestApplyWarpAtLandmark)
		{
		}

		TEST_METHOD(TestApplyInverseWarpAtLandmark)
		{
		}

		TEST_METHOD(TestResampleUniformImage)
		{
		}

		TEST_METHOD(TestResampleImageHistogram)
		{
		}

	};
}
