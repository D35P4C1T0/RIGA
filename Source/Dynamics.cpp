#include "Dynamics.h"
#include "Circuit.h"
#include <algorithm>
#include <cmath>
#include <cstring>
namespace strip {
namespace {
constexpr float junction[512] = {0.0f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096100045e-06f,
                                 3.096799901e-06f,
                                 3.102599976e-06f,
                                 3.156899993e-06f,
                                 3.663699999e-06f,
                                 8.383900422e-06f,
                                 5.199389852e-05f,
                                 0.0004271928046f,
                                 0.002606458263f,
                                 0.00830040127f,
                                 0.01640964113f,
                                 0.02577351592f,
                                 0.03578856215f,
                                 0.04618864879f,
                                 0.05683914199f,
                                 0.06766401976f,
                                 0.07861667871f,
                                 0.08966670185f,
                                 0.1007932201f,
                                 0.1119813174f,
                                 0.1232200116f,
                                 0.1345009953f,
                                 0.1458178014f,
                                 0.1571653485f,
                                 0.1685395688f,
                                 0.1799370944f,
                                 0.1913552135f,
                                 0.2027916163f,
                                 0.2142443657f,
                                 0.2257118076f,
                                 0.2371925712f,
                                 0.2486854047f,
                                 0.260189265f,
                                 0.2717032135f,
                                 0.2832264304f,
                                 0.2947582304f,
                                 0.3062979281f,
                                 0.3178449869f,
                                 0.3293988705f,
                                 0.3409591615f,
                                 0.3525253832f,
                                 0.364097178f,
                                 0.3756742477f,
                                 0.3872562349f,
                                 0.3988428712f,
                                 0.4104338586f,
                                 0.4220290184f,
                                 0.4336281121f,
                                 0.4452309012f,
                                 0.4568372369f,
                                 0.4684469402f,
                                 0.4800598323f,
                                 0.4916757643f,
                                 0.5032945871f,
                                 0.5149161816f,
                                 0.5265404582f,
                                 0.5381672978f,
                                 0.5497965217f,
                                 0.5614280701f,
                                 0.5730618834f,
                                 0.5846978426f,
                                 0.5963358879f,
                                 0.6079759002f,
                                 0.6196178198f,
                                 0.6312615871f,
                                 0.642907083f,
                                 0.6545543671f,
                                 0.6662032604f,
                                 0.6778537631f,
                                 0.6895057559f,
                                 0.7011592984f,
                                 0.7128142715f,
                                 0.7244706154f,
                                 0.7361283898f,
                                 0.747787416f,
                                 0.7594476938f,
                                 0.7711092234f,
                                 0.782771945f,
                                 0.7944357991f,
                                 0.8061008453f,
                                 0.8177669644f,
                                 0.8294340968f,
                                 0.8411023021f,
                                 0.8527715206f,
                                 0.8644416928f,
                                 0.8761128783f,
                                 0.8877848983f,
                                 0.8994578719f,
                                 0.9111317396f,
                                 0.9228064418f,
                                 0.9344819784f,
                                 0.9461583495f,
                                 0.9578355551f,
                                 0.9695134759f,
                                 0.9811921716f,
                                 0.9928715825f,
                                 1.004551768f,
                                 1.01623261f,
                                 1.027914166f,
                                 1.039596438f,
                                 1.051279306f,
                                 1.06296289f,
                                 1.074647069f,
                                 1.086331844f,
                                 1.098017216f,
                                 1.109703302f,
                                 1.121389866f,
                                 1.133077025f,
                                 1.144764662f,
                                 1.156452894f,
                                 1.168141723f,
                                 1.179831028f,
                                 1.19152081f,
                                 1.203211188f,
                                 1.214901924f,
                                 1.226593256f,
                                 1.238285065f,
                                 1.24997735f,
                                 1.261669993f,
                                 1.273363113f,
                                 1.28505671f,
                                 1.296750784f,
                                 1.308445215f,
                                 1.320140123f,
                                 1.331835389f,
                                 1.343531132f,
                                 1.355227113f,
                                 1.36692369f,
                                 1.378620505f,
                                 1.390317798f,
                                 1.402015328f,
                                 1.413713336f,
                                 1.425411701f,
                                 1.437110305f,
                                 1.448809385f,
                                 1.460508704f,
                                 1.4722085f,
                                 1.483908534f,
                                 1.495608807f,
                                 1.507309556f,
                                 1.519010544f,
                                 1.530711889f,
                                 1.542413473f,
                                 1.554115415f,
                                 1.565817595f,
                                 1.577520132f,
                                 1.589222908f,
                                 1.600926042f,
                                 1.612629414f,
                                 1.624333024f,
                                 1.636036873f,
                                 1.647741079f,
                                 1.659445524f,
                                 1.671150208f,
                                 1.682855248f,
                                 1.694560409f,
                                 1.706265926f,
                                 1.717971563f,
                                 1.729677558f,
                                 1.741383791f,
                                 1.753090262f,
                                 1.764796853f,
                                 1.776503801f,
                                 1.788210869f,
                                 1.799918294f,
                                 1.811625838f,
                                 1.823333621f,
                                 1.835041642f,
                                 1.846749902f,
                                 1.858458281f,
                                 1.870166898f,
                                 1.881875753f,
                                 1.893584847f,
                                 1.905294061f,
                                 1.917003512f,
                                 1.928713202f,
                                 1.940423012f,
                                 1.95213306f,
                                 1.963843226f,
                                 1.975553632f,
                                 1.987264276f,
                                 1.998975039f,
                                 2.010685921f,
                                 2.022397041f,
                                 2.0341084f,
                                 2.045819759f,
                                 2.057531595f,
                                 2.069243431f,
                                 2.080955267f,
                                 2.09266758f,
                                 2.104379892f,
                                 2.116092205f,
                                 2.127804995f,
                                 2.139517784f,
                                 2.151230574f,
                                 2.16294384f,
                                 2.174657106f,
                                 2.186370373f,
                                 2.198084116f,
                                 2.209797859f,
                                 2.221511602f,
                                 2.233225584f,
                                 2.244939804f,
                                 2.256654024f,
                                 2.268368483f,
                                 2.280083179f,
                                 2.291797876f,
                                 2.303512573f,
                                 2.315227747f,
                                 2.326942682f,
                                 2.338658094f,
                                 2.350373507f,
                                 2.362088919f,
                                 2.373804569f,
                                 2.385520458f,
                                 2.397236347f,
                                 2.408952236f,
                                 2.420668364f,
                                 2.432384729f,
                                 2.444101095f,
                                 2.455817699f,
                                 2.467534304f,
                                 2.479251146f,
                                 2.490967989f,
                                 2.502684832f,
                                 2.514402151f,
                                 2.526119232f,
                                 2.537836552f,
                                 2.54955411f,
                                 2.561271667f,
                                 2.572989225f,
                                 2.584707022f,
                                 2.596424818f,
                                 2.608142853f,
                                 2.619860888f,
                                 2.631579161f,
                                 2.643297434f,
                                 2.655015945f,
                                 2.666734457f,
                                 2.678452969f,
                                 2.690171719f,
                                 2.701890469f,
                                 2.713609457f,
                                 2.725328445f,
                                 2.737047672f,
                                 2.748766661f,
                                 2.760486126f,
                                 2.772205353f,
                                 2.783924818f,
                                 2.795644522f,
                                 2.807364225f,
                                 2.819083929f,
                                 2.830803871f,
                                 2.842523813f,
                                 2.854243755f,
                                 2.865963936f,
                                 2.877684116f,
                                 2.889404297f,
                                 2.901124716f,
                                 2.912845135f,
                                 2.924565792f,
                                 2.936286449f};
constexpr float constant[14] = {0.04596945643f,  1.723854661f,  0.223163411f,   0.05281959847f,
                                5.927552223f,    4.189482689f,  -0.9900990129f, -0.1425744444f,
                                -0.06895418465f, 0.7814807296f, 0.9663564563f,  0.1111111119f,
                                1.100000024f,    2.48600316f};
constexpr float vca[2][5] = {{1.001959443f, 0.0003991180565f, -0.002613381017f, -11.25401211f, 11.35582638f},
                             {1.000246644f, 0.0008935140213f, -0.0003290051827f, -30.9416256f, 32.75216293f}};
constexpr double taper[] = {0,   .01, .04, .1,  .14, .19, .23, .28, .34, .4,  .45, .5,
                            .55, .6,  .66, .72, .77, .81, .86, .9,  .96, .99, 1};
constexpr double releaseTaper[] = {0,   .01,  .02, .043, .068, .092, .115, .135, .16, .18, .205, .23,
                                   .25, .275, .36, .46,  .55,  .64,  .72,  .81,  .9,  .96, 1};
// Electrical control travel is interpolated before the passive component networks.
double travel(double n, const double *curve) {
    const double x = std::clamp(n, 0.0, 1.0) * 22;
    const auto i = std::min(static_cast<int>(x), 21);
    return curve[i] + (curve[i + 1] - curve[i]) * (x - i);
}
float rate(float reference, double sampleRate) {
    return static_cast<float>(-std::expm1(std::log1p(-static_cast<double>(reference)) * 48000 / sampleRate));
}
float diode(float voltage) {
    if (voltage > 3)
        return voltage;
    const float x = std::clamp(voltage * 0.1666666f + 0.4999998f, 0.0f, 1.0f) * 511;
    const auto i = static_cast<unsigned>(x), j = std::min(i + 1, 511u);
    return junction[i] + (junction[j] - junction[i]) * (x - static_cast<float>(i));
}
float logarithm(float x) {
    std::uint32_t bits{};
    std::memcpy(&bits, &x, sizeof(bits));
    const int exponent = static_cast<int>((bits >> 23) & 255) - 128;
    bits = (bits & 0xbfffffffu) | 0x3f800000u;
    float mantissa{};
    std::memcpy(&mantissa, &bits, sizeof(bits));
    return (((2.0f - mantissa * 0.33333334f) * mantissa + static_cast<float>(exponent)) - 0.6666667f) *
           0.6931472f;
}
float exponential(float x) {
    const float y = x * 1.442695f;
    const int whole = static_cast<int>(y < 0 ? y - 0.99999994f : y);
    const float fraction = y - static_cast<float>(whole);
    return std::ldexp(((fraction * 0.05550411f + 0.2402265f) * fraction + 0.6931472f) * fraction + 1.0f,
                      whole);
}
std::array<float, 5> coefficients(double b0, double b1, double b2, double a1, double a2) {
    const double reflection = a1 / (1 + a2), feed = b1 - a1 * b2;
    return {static_cast<float>(b0 - feed * reflection - b2 * a2), static_cast<float>(feed),
            static_cast<float>(reflection), static_cast<float>(b2), static_cast<float>(a2)};
}
std::array<float, 5> highPass(double sampleRate, double frequency, double q, double level = 1) {
    constexpr double pi = 3.14159265358979323846;
    const double angle = std::min(2 * pi * frequency, sampleRate * pi * .65),
                 k = std::tan(angle * (.5 / sampleRate));
    const double f2 = frequency * frequency, fs2 = sampleRate * sampleRate;
    const double term = f2 * 1558.5454565440389 * q * f2 * q;
    const double gain = (fs2 * 9.869604401089358 * q * level) /
                        std::sqrt(fs2 * f2 * 389.6363641360097 - q * fs2 * f2 * q * 779.2727282720194 + term +
                                  q * q * 97.40909103400243 * fs2 * fs2);
    const double damp =
        1 / ((q * level * angle * angle) / (std::sqrt(angle * angle * f2 * 39.47841760435743 -
                                                      q * q * f2 * angle * angle * 78.95683520871486 +
                                                      angle * angle * q * angle * angle * q + term) *
                                            gain));
    const double a0 = (damp + k) * k + 1;
    return coefficients(gain / a0, -2 * gain / a0, gain / a0, 2 * (k * k - 1) / a0,
                        (k * k - damp * k + 1) / a0);
}

} // namespace
float Dynamics::Lattice::process(float input) {
    const float residual = input - first * coefficient[4];
    const float core = residual - second * coefficient[2];
    const float outer = residual * coefficient[4] + first;
    first = core * coefficient[2] + second;
    second = core;
    return first * coefficient[1] + outer * coefficient[3] + core * coefficient[0];
}
void Dynamics::prepare(double sampleRate, int channels) {
    mono_ = channels == 1;
    controls_.initialized = false;
    sampleRate_ = std::isfinite(sampleRate) && sampleRate > 0 ? sampleRate : 48000;
    constexpr double pi = 3.14159265358979323846, f = .07234315595086152;
    const double w2 = f * f * 39.47841760435743;
    const double gain = sampleRate_ * pi / std::sqrt(sampleRate_ * sampleRate_ * pi * pi + w2),
                 square = gain * gain;
    const double fourth = square * square * .5000000000000001;
    double pole = std::tan(std::sqrt(w2 / (1 - fourth)) * square * .7071067811865476 * (.5 / sampleRate_));
    pole *= std::sqrt(square - fourth) / (square * .7071067811865476);
    const double b0 = gain / (1 + pole), b1 = -b0, k = (pole - 1) / (pole + 1);
    dc_ = {static_cast<float>(b0 - b1 * k), static_cast<float>(b1), static_cast<float>(k)};
    setParameters(parameters_);
    reset();
}
void Dynamics::reset() {
    for (auto &c : channels_) {
        c.keyFilter.first = c.keyFilter.second = c.compressorFilter.first = c.compressorFilter.second = 0;
        c.compressorPower = c.gatePower = c.first = c.second = c.idle = c.average = c.gateState =
            c.switchState = c.dcState = 0;
    }
    compressorMeter_ = gateMeter_ = 0;
}
void Dynamics::setParameters(const Parameters &parameters, bool immediate) {
    if (immediate)
        controls_.initialized = false;
    parameters_ = parameters;
    auto p = [&](Param name) { return std::clamp(double(parameters[index(name)]), 0.0, 1.0); };
    const auto fast = p(Param::compFast) >= 0.5 ? 1 : 0;
    const double release = travel(p(Param::compRelease), releaseTaper);
    const double ratio = travel(p(Param::ratio), taper);
    const double threshold = travel(1 - p(Param::compThreshold), taper);
    const double gateThreshold = travel(p(Param::gateThreshold), taper);
    const double gateRelease = travel(p(Param::gateRelease), releaseTaper);
    detector_ = rate(constant[2], sampleRate_);
    idleRate_ = rate(0.0003693171311169863f, sampleRate_);
    averageRate_ = rate(0.0044228145852684975f, sampleRate_);
    for (std::size_t channel = 0; channel < 2; ++channel) {
        auto &c = channels_[channel];
        const int selected = static_cast<int>(std::round(p(Param::channel) * 71));
        const int firstChannel = selected == 0 ? 0 : selected + 72;
        const int variation = channel == 0 || p(Param::stereoMode) >= .5 ? firstChannel
                              : firstChannel == 0                        ? 72
                                                                         : firstChannel + 1;
        const auto R = [variation](double value) { return circuitComponent(value, variation); };
        const auto parallel = [](double a, double b) { return a * b / (a + b); };
        const auto C = [variation](double value) { return circuitComponent(value, variation, true); };
        const auto charging = [this](double seconds) {
            return static_cast<float>(1 - std::exp(-1 / (sampleRate_ * seconds)));
        };
        const double firstCap = C(3.3e-6), secondCap = C(2.2e-5);
        double attackResistance = R(10000);
        if (fast)
            attackResistance = parallel(attackResistance, R(300));
        const double bridge = R(10000), bleed = R(1000000), releaseResistance = release * 250000;
        const double junction = releaseResistance * bleed + (releaseResistance + bleed) * bridge;
        const double lower = releaseResistance * attackResistance + junction;
        const double total = bleed * attackResistance + lower;
        c.capacitor = {static_cast<float>(junction / total),
                       static_cast<float>(lower / total),
                       charging(attackResistance * firstCap),
                       charging((total / (releaseResistance + bleed)) * firstCap),
                       charging((total / bleed) * firstCap),
                       charging(((junction * attackResistance) / lower) * secondCap),
                       charging((total / (bleed + attackResistance + bridge)) * secondCap),
                       charging((total / bleed) * secondCap)};
        constexpr double voltageScale = 8.70142984443495, bias = 5.92755214050494;
        const double ratioPot = R(10000), ratioTap = (p(Param::compMix) == 0 ? 0 : ratio) * ratioPot,
                     remaining = ratioPot - ratioTap;
        const double thresholdPot = R(10000), thresholdTap = threshold * thresholdPot;
        const double amplification =
            R(8200) * (R(100000) + R(10)) /
                ((parallel(R(56000), R(10) * 2) + R(5300)) * (R(8200) + R(100000) + R(10))) +
            1;
        c.ratio =
            static_cast<float>(amplification * ratioTap / (parallel(R(2700), remaining) + R(100) + ratioTap));
        const double ratioBranch = parallel(R(68000), ratioTap);
        const double sharedBranch = parallel(R(2700), remaining) + ratioBranch;
        const double thresholdBranch = parallel(sharedBranch, thresholdPot - thresholdTap);
        c.offset = static_cast<float>(((-3.8 / (thresholdBranch + thresholdTap)) * thresholdBranch *
                                       ratioBranch / sharedBranch / R(68000)) *
                                      R(30100) / voltageScale);
        const double biasBranch = parallel(R(4700) + thresholdTap, R(1050000));
        c.threshold = static_cast<float>(
            ((5 / (biasBranch + thresholdPot - thresholdTap)) * (biasBranch / R(1050000)) * R(1000000) -
             bias) /
            voltageScale);
        const double gatePot = R(25000), gateTap = gateThreshold * gatePot;
        const double gateBranch = parallel(R(36000), R(6800) + gateTap);
        double gateBiasValue =
            (((5 / (gateBranch + gatePot - gateTap)) * (gateBranch / R(36000)) * R(36000) - bias) +
             (8 / R(533000)) * R(36000)) /
            voltageScale;
        if (p(Param::gateThresholdRange) >= .5)
            gateBiasValue -= .369;
        c.gateThreshold = static_cast<float>(gateBiasValue);
        c.range = static_cast<float>(travel(p(Param::gateRange), taper) * 0.6205876469612122);
        const double gateCap = C(6.8e-6);
        c.gateAttack = charging(R(p(Param::gateFast) >= .5 ? 1 : 100) * gateCap);
        c.gateRelease = charging((gateRelease * R(250000) + R(18500)) * gateCap);
        const double fixedFirst = circuitComponent(6200, firstChannel),
                     fixedSecond = circuitComponent(10000, firstChannel);
        c.keyFilter.coefficient = highPass(sampleRate_, 10, 1, fixedSecond / (fixedSecond + fixedFirst));
        // The detector includes the minimum high-pass corner even at zero travel.
        const double frequency = 10 * std::pow(200.0, p(Param::sidechainHP));
        c.compressorFilter.coefficient = highPass(sampleRate_, frequency, 0.7071067811865475);
    }
    std::copy(std::begin(vca[p(Param::vcaMode) >= .5 ? 1 : 0]),
              std::end(vca[p(Param::vcaMode) >= .5 ? 1 : 0]), vca_.begin());
    controls_.set({channels_[0].ratio, channels_[1].ratio, channels_[0].offset, channels_[1].offset,
                   static_cast<float>(p(Param::compMix)), static_cast<float>(p(Param::gateInvert)),
                   static_cast<float>(p(Param::dynamicsOn)), vca_[0], vca_[1], vca_[2], vca_[3], vca_[4]});
}
void Dynamics::process(float &left, float &right, float keyLeft, float keyRight) {
    float compressor[2]{}, gate[2]{};
    const float key[2]{keyLeft, keyRight};
    const bool gateMode = parameters_[index(Param::gateMode)] >= .5f;
    const auto &smooth = controls_.current;
    const float hysteresis = parameters_[index(Param::hysteresis)] * .31f;
    const float secondThreshold = parameters_[index(Param::secondThreshold)] * 1e-6f;
    for (std::size_t i = 0; i < 2; ++i) {
        auto &c = channels_[i];
        const float gateKey = c.keyFilter.process(key[i]);
        const float compKey = c.compressorFilter.process(gateKey);
        c.compressorPower += (std::max(compKey * compKey, 1e-14f) - c.compressorPower) * detector_;
        const float excitation = std::max(logarithm(c.compressorPower) * constant[3] - c.threshold, 0.0f);
        c.average += (excitation - c.average) * averageRate_;
        const auto &a = c.capacitor;
        const float rectified = std::max(excitation - c.first * a[0] - c.second * a[1], 0.0f);
        if (rectified == 0)
            c.idle -= (c.idle + 1.723854f) * idleRate_;
        else
            c.idle = 0;
        const float leakage = c.idle < .5f && c.average < secondThreshold ? 1.7e-5f : 0;
        c.first += ((rectified * a[2] - c.first * a[3]) - c.second * a[4]) - leakage;
        c.second += (rectified * a[5] - c.second * a[6]) - c.first * a[7];
        compressor[i] = smooth[i] * diode(c.first + c.second);
        c.gatePower += (std::max(gateKey * gateKey, 1e-14f) - c.gatePower) * detector_;
        const float below = std::min(logarithm(c.gatePower) * constant[3] - c.gateThreshold, 0.0f);
        const float linear = below * constant[6];
        const float switchVoltage =
            std::clamp((c.switchState - constant[7]) * constant[10] + constant[7], constant[8], constant[9]);
        const float switchingThreshold = (switchVoltage - constant[7]) * constant[11] + constant[7];
        if (below >= switchingThreshold)
            c.switchState = -constant[1];
        else if (below < switchingThreshold - hysteresis)
            c.switchState = constant[1];
        const float drive = (gateMode ? diode(switchVoltage) : linear) * constant[12];
        const float pull = std::min(((drive - constant[0]) - constant[0]) - c.gateState + constant[0], 0.0f);
        c.gateState += pull * c.gateAttack + (c.range - c.gateState) * c.gateRelease;
        gate[i] = constant[13] * c.gateState;
        gate[i] = gate[i] + (c.range - 2 * gate[i]) * smooth[5];
    }
    if (mono_)
        compressor[1] = compressor[0];
    if (parameters_[index(Param::compLink)] >= .5f)
        compressor[0] = compressor[1] = std::max(compressor[0], compressor[1]);
    const float mix = smooth[4];
    float *audio[2]{&left, &right};
    float maximumComp = 0, maximumGate = 0;
    for (std::size_t i = 0; i < 2; ++i) {
        const float voltage = diode(std::max(compressor[i], gate[i])) + smooth[2 + i];
        float gain = exponential(-voltage * constant[5]);
        if (compressor[i] >= gate[i])
            gain = (gain - 1) * mix + 1;
        maximumComp = std::max(maximumComp, compressor[i]);
        maximumGate = std::max(maximumGate, gate[i]);
        {
            const float dry = *audio[i];
            const float x = std::clamp(*audio[i] * gain, smooth[10], smooth[11]);
            const float square = x * x, cube = x * x * x;
            const float shaped = square * smooth[8] + x * smooth[7] + cube * smooth[9];
            const float core = shaped - channels_[i].dcState * dc_[2];
            *audio[i] = (core * dc_[2] + channels_[i].dcState) * dc_[1] + dc_[0] * core;
            channels_[i].dcState = core;
            if (smooth[6] != 1)
                *audio[i] = *audio[i] * smooth[6] + dry * (1 - smooth[6]);
        }
    }
    compressorMeter_ = maximumComp * constant[5] * 8.685889638f;
    gateMeter_ = maximumGate * constant[5] * 8.685889638f;
}
} // namespace strip
