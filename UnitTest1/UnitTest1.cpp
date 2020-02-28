#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace WarpTpsLib
{
	namespace UnitTest
	{
		TEST_CLASS(TPSTransformUnitTest)
		{
		public:
			TEST_METHOD(TestAddLandmark)
			{
				Logger::WriteMessage("TestAddLandmark");

				auto tpsTransform = new CTPSTransform();
				tpsTransform->AddLandmark(CVectorD<3>(0.0, 0.0, 0.0));
				tpsTransform->AddLandmark(CVectorD<3>(1.0, 0.0, 0.0));
				tpsTransform->AddLandmark(CVectorD<3>(0.0, 1.0, 0.0));
				tpsTransform->AddLandmark(CVectorD<3>(0.25, 0.5, 0.0));

				Assert::IsTrue(tpsTransform->GetLandmark<0>(0).IsApproxEqual(CVectorD<3>(0.0, 0.0, 0.0)));
				Assert::IsTrue(tpsTransform->GetLandmark<1>(0).IsApproxEqual(CVectorD<3>(0.0, 0.0, 0.0)));

				Assert::IsTrue(tpsTransform->GetLandmark<0>(1).IsApproxEqual(CVectorD<3>(1.0, 0.0, 0.0)));
				Assert::IsTrue(tpsTransform->GetLandmark<1>(1).IsApproxEqual(CVectorD<3>(1.0, 0.0, 0.0)));

				Assert::IsTrue(tpsTransform->GetLandmark<0>(2).IsApproxEqual(CVectorD<3>(0.0, 1.0, 0.0)));
				Assert::IsTrue(tpsTransform->GetLandmark<1>(2).IsApproxEqual(CVectorD<3>(0.0, 1.0, 0.0)));

				Assert::IsTrue(tpsTransform->GetLandmark<0>(3).IsApproxEqual(CVectorD<3>(0.25, 0.5, 0.0)));
				Assert::IsTrue(tpsTransform->GetLandmark<1>(3).IsApproxEqual(CVectorD<3>(0.25, 0.5, 0.0)));

				Logger::WriteMessage("Done TestAddLandmark");
			}

			TEST_METHOD(TestCalculateWarp)
			{
				Logger::WriteMessage("TestAddLandmark");

				auto tpsTransform = new CTPSTransform();
				tpsTransform->AddLandmark(CVectorD<3>(0.0, 0.0, 0.0));
				tpsTransform->AddLandmark(CVectorD<3>(1.0, 0.0, 0.0));
				tpsTransform->AddLandmark(CVectorD<3>(0.0, 1.0, 0.0));
				tpsTransform->AddLandmark(CVectorD<3>(0.5, 0.5, 0.0));

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

			TEST_METHOD(TestMatrixProduct)
			{
				std::stringstream ss;

				// test that ublas initialization is all zeros
				ublas::vector<REAL> vZero(10 + 3);
				Assert::IsTrue(std::all_of(vZero.begin(), vZero.end(), [](double f) {return f == 0.0 || f != 0.0;}));

				auto scaleBy = 2.0;
				ublas::matrix<REAL> m(3, 3);
				m(0, 0) = 1.0; m(0, 1) = 0.0; m(0, 2) = 0.0;
				m(1, 0) = 0.0; m(1, 1) = -scaleBy; m(1, 2) = 0.0;
				m(2, 0) = 0.0; m(2, 1) = 0.0; m(2, 2) = 1.0;
				ss << "m" << m << std::endl;

				CVectorN<REAL> v(3);
				v[0] = 1.0;
				v[1] = -2.0;
				v[2] = 3.0;
				ss << "original v" << v.as_vector() << std::endl;

				auto m_times_v_ublas = ublas::prod(m, v.as_vector());
				ss << "m x v (ublas)" << m_times_v_ublas << std::endl;

				TRACE(ss.str().c_str());

				Assert::IsTrue(v[0] == m_times_v_ublas(0), L"v[0] == m_times_v_ublas(0)");
				Assert::IsTrue(m_times_v_ublas(1) == -scaleBy * v[1], L"m_times_v_ublas(1) == -scaleBy * v[1]");
				Assert::IsTrue(m_times_v_ublas(2) == v[2], L"m_times_v_ublas(2) == v[2]");
			}
		};
	}
}
