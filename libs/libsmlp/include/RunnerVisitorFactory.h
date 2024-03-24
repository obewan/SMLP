/*****************************************************************//**
 * \file    RunnerVisitorFactory.h
 * \brief   RunnerVisitor Factory
 * 
 * \author  Damien Balima (https://dams-labs.net)
 * \date    March 2024
 * \copyright CC-BY-NC-SA-4.0 (https://creativecommons.org/licenses/by-nc-sa/4.0/)
 *********************************************************************/
#pragma once
#include "RunnerFileVisitor.h"
#include "RunnerSocketVisitor.h"
#include "RunnerStdinVisitor.h"
#include <memory>

namespace smlp {
	class RunnerVisitorFactory {
	public:
		const RunnerVisitor& getRunnerFileVisitor() {
			if (!runnerFileVisitor_) {
				runnerFileVisitor_ = std::make_unique<RunnerFileVisitor>();
			}
			return *runnerFileVisitor_;
		}
		const RunnerVisitor& getRunnerSocketVisitor() {
			if (!runnerSocketVisitor_) {
				runnerSocketVisitor_ = std::make_unique<RunnerSocketVisitor>();
			}
			return *runnerSocketVisitor_;
		}
		const RunnerVisitor& getRunnerStdinVisitor() {
			if (!runnerStdinVisitor_) {
				runnerStdinVisitor_ = std::make_unique<RunnerStdinVisitor>();
			}
			return *runnerStdinVisitor_;
		}
	private:
		std::unique_ptr<RunnerFileVisitor> runnerFileVisitor_ = nullptr;
		std::unique_ptr<RunnerSocketVisitor> runnerSocketVisitor_ = nullptr;
		std::unique_ptr<RunnerStdinVisitor> runnerStdinVisitor_ = nullptr;
	};
} // namespace smlp

