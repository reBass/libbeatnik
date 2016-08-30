// Copyright (c) 2016 Roman Beránek. All rights reserved.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
