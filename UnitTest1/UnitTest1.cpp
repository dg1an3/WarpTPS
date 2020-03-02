#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace WarpTpsLib
{
	namespace UnitTest
	{
		ostream& operator<<(ostream& os, const CVectorD<3,REAL>::Point_t& pt)
		{
			os << pt.get<0>() << ',' << pt.get<1>() << pt.get<2>();
			return os;
		}

		TEST_CLASS(TPSTransformUnitTest)
		{
		public:

			TEST_METHOD(TestMatrixProduct)
			{
				std::stringstream ss;

				// test that ublas initialization is all zeros
				ublas::vector<REAL> vZero(10 + 3);
				Assert::IsTrue(std::all_of(vZero.begin(), vZero.end(), [](double f) {return f == 0.0 || f != 0.0; }));

				auto scaleBy = 2.0;
				ublas::matrix<REAL> m(3, 3);
				m(0, 0) = 1.0; m(0, 1) = 0.0; m(0, 2) = 0.0;
				m(1, 0) = 0.0; m(1, 1) = -scaleBy; m(1, 2) = 0.0;
				m(2, 0) = 0.0; m(2, 1) = 0.0; m(2, 2) = 1.0;
				ss << "m" << m << std::endl;

				ublas::vector<REAL> v(3);
				v(0) = 1.0;
				v(1) = -2.0;
				v(2) = 3.0;
				ss << "original v" << v << std::endl;

				auto m_times_v_ublas = ublas::prod(m, v);
				ss << "m x v (ublas)" << m_times_v_ublas << std::endl;

				TRACE(ss.str().c_str());

				Assert::IsTrue(v(0) == m_times_v_ublas(0), L"v[0] == m_times_v_ublas(0)");
				Assert::IsTrue(m_times_v_ublas(1) == -scaleBy * v(1), L"m_times_v_ublas(1) == -scaleBy * v[1]");
				Assert::IsTrue(m_times_v_ublas(2) == v(2), L"m_times_v_ublas(2) == v[2]");
			}


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

			// tests that warp at landmarks are at the same point
			TEST_METHOD(TestWarpAtLandmarks)
			{
				Logger::WriteMessage("TestWarpAtLandmarks");

				auto tpsTransform = new CTPSTransform();
				tpsTransform->AddLandmark(CVectorD<3>(-1.0, -1.0, 0.0));
				tpsTransform->AddLandmark(CVectorD<3>(-1.0, 1.1, 0.0));
				tpsTransform->AddLandmark(CVectorD<3>(1.1, 1.0, 0.0));
				tpsTransform->AddLandmark(CVectorD<3>(0.9, -1.0, 0.0));

				// add divergent landmark
				tpsTransform->AddLandmark(CVectorD<3>(0.0, 0.0, 0.0), CVectorD<3>(0.1,-0.2,0.0));

				for (int nLandmark = 0; nLandmark < tpsTransform->GetLandmarkCount(); nLandmark++)
				{
					std::stringstream ss;

					CVectorD<3, REAL> vLandmark0 = tpsTransform->GetLandmark<0>(nLandmark);
					ss << "Landmark<0> at " << nLandmark << " = " << vLandmark0.point() << endl;
					FlushStream(ss);

					// note that Eval transforms the point to an offset, which then needs to be added to the other landmark
					CVectorD<3, REAL>::Point_t vTransformed(0.0);
					tpsTransform->Eval(vLandmark0.point(), vTransformed, 1.0);
					bg::add_point(vTransformed, vLandmark0.point());
					ss << "Landmark<0> at " << nLandmark << " tranforms to " << vTransformed << endl;
					FlushStream(ss);

					CVectorD<3, REAL> vLandmark1 = tpsTransform->GetLandmark<1>(nLandmark);
					ss << "Landmark<1> at " << nLandmark << " = " << vLandmark1.point() << endl;
					FlushStream(ss);

					Assert::IsTrue(bg::equals(vTransformed, vLandmark1.point()));
				}

				Logger::WriteMessage("Done TestWarpAtLandmarks");
			}

			TEST_METHOD(TestInverseWarpAtLandmark)
			{
			}

			void FlushStream(std::stringstream& stream)
			{
				Logger::WriteMessage(stream.str().c_str()); 
				stream.clear();
			}
		};
	}
}
