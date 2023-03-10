# quantpp

A just-for-fun project in C++ on option pricing.

## Features

We will be interested in combinations of the following models, options and valuation methods:
| Model      | Option      | Technique                |
| ---------- | ----------- | ------------------------ |
| BS         | European    | Monte Carlo method       |
| Heston     | American    | Finite difference method |

A list of currently implemented features is presented below:
- [ ] **Black-Scholes** model:
  - [ ] **European** vanilla:
    - [ ] analytical option pricing.
    - [x] **MC** option pricing.
    - [ ] **PDE** option pricing.
  - [ ] **American** vanilla:
    - [ ] **MC** option pricing.
    - [ ] **PDE** option pricing.
- [ ] **Heston** model:
  - [ ] **European** vanilla:
    - [ ] **MC** option pricing.
    - [ ] **PDE** option pricing.
  - [ ] **American** vanilla:
    - [ ] **MC** option pricing.
    - [ ] **PDE** option pricing.

## Requirements 
- C++17
- make

## Building
- **Debug** version (default):
```bash
make
```
or
```bash
make all
```
or
```bash 
make debug
```

- **Release** version (optimized compilation):
```bash
make release
```

## Running:
```bash
./build/target/quantpp_app
```
or
```bash
make run
```
### Configuration
The following command line args can be set when running from the executable directly [using make run]:
- BS risk-free interest rate: --rate [RATE=]
- BS volatility: --vol [VOL=]
- spot price: --spot [SPOT=]
- strike value: --strike [STRIKE=]
- time-to-maturity: --ttm [TTM=]
- simulation paths: --npaths [NPATHS=]
- simulation steps: --nsteps [NSTEPS=]

E.g. the following two commands are equivalent:
```bash
./build/target/quantpp_app --rate 0.06 --vol 0.22 --spot 110 --strike 90 --ttm 1.2 --npaths 50000 --nsteps 240
```
or
```bash
make run RATE=0.06 VOL=0.22 SPOT=110 STRIKE=90 TTM=1.2 NPATHS=50000 NSTEPS=240
```