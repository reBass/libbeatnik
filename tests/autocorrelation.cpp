// Copyright (c) 2016-2018 Roman Beránek. All rights reserved.
//
// Released under MIT license. See LICENSE file.

#include <beatnik/fft/ACF.hpp>

using namespace reBass;

int main() {
    ACF<float, 64> acf;

    std::array<float, 64> input;
    input.fill(0);
    for (auto i = 0; i < 64; i += 13) {
        input[i] = 1;
    }

    acf.compute(input, input);


    std::array<float, 16> input2;
    input2.fill(0);
    input2[2] = 1;
    std::array<std::complex<float>, 9> output2;
    Real_FFT<float, 16> fft;
    fft.transform_forward(input2, output2);
    fft.transform_backward(output2, input2);
    return 0;
}
