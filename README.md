## Libbeatnik
C++17 library implementing a real-time beat tracking algorithm. Loosely based upon Adam Stark's [btrack](https://github.com/adamstark/BTrack).

### Resources
* Stark, A. M., Davies, M. E. P. and Plumbley, M. D. (2009) [Real-Time Beat-Synchronous Analysis of Musical Audio](http://www.elec.qmul.ac.uk/people/markp/2009/StarkDaviesPlumbley09-dafx.pdf). _Proceedings of the 12th International Conference on Digital Audio Effects (DAFx-09)_
* Stark, A. M. (2011) [Musicians and Machines: Bridging the Semantic Gap in Live Performance](https://adamstark.co.uk/phd-thesis/), Chapter 3, PhD Thesis, Queen Mary, University of London.
* Ellis, D. P. W. (2008) Beat Tracking by Dynamic Programming, _Journal of New Music Research_, 36:1, 51-60, DOI: [10.1080/09298210701653344](http://dx.doi.org/10.1080/09298210701653344)
* Davies, M. E. P. and Plumbley, M. D. (2007). Context-dependent beat tracking of musical audio. _IEEE Transactions on Audio, Speech and Language Processing_, 15(3):1009–1020. DOI: [10.1109/TASL.2006.885257](https://doi.org/10.1109/TASL.2006.885257)

## Compilation
Requires `cmake 3.11+`

Example using make

    git clone --recursive https://github.com/reBass/libbeatnik.git
    cd libbeatnik
    mkdir -p build
    cd build
    cmake ..
    make
    
