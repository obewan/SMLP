# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog],
and this project adheres to [Semantic Versioning].

## [1.2.1] - 2024-03-24

### Fixed

- fix Windows compilation warnings
- fix Windows integrity tests
- fix version
- some code refactoring

## [1.2.0] - 2024-03-21

### Changed

- big code refactoring. Neural network parameters untouched.

## [1.1.0] - 2024-02-29

### Changed

- json model metadata has been refactored. Null vectors of neurons replaced by neurons count.  
  Before: { "neurons": [ null, null, null ] }  
  After: { "neurons": 3 }  
  Be sure to change your json network model if using the v1.0.0 with the v1.1.0.

## [1.0.0] - 2024-02-27

- initial release

<!-- Links -->

[keep a changelog]: https://keepachangelog.com/en/1.0.0/
[semantic versioning]: https://semver.org/spec/v2.0.0.html

<!-- Versions -->

[unreleased]: https://github.com/obewan/SMLP/compare/v1.2.1...HEAD
[1.2.1]: https://github.com/obewan/SMLP/compare/v1.2.0..v1.2.1
[1.2.0]: https://github.com/obewan/SMLP/compare/v1.1.0..v1.2.0
[1.1.0]: https://github.com/obewan/SMLP/compare/v1.0.0..v1.1.0
[1.0.0]: https://github.com/obewan/SMLP/releases/tag/v1.0.0
