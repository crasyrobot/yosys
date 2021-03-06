/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2018 Icenowy Zheng <icenowy@aosc.io>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include "kernel/yosys.h"
#include "kernel/sigtools.h"

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

struct AnlogicDetermineInitPass : public Pass {
	AnlogicDetermineInitPass() : Pass("anlogic_determine_init", "Anlogic: Determine the init value of cells") { }
	void help() YS_OVERRIDE
	{
		log("\n");
		log("    anlogic_determine_init [selection]\n");
		log("\n");
		log("Determine the init value of cells that doesn't allow unknown init value.\n");
		log("\n");
	}

	Const determine_init(Const init)
	{
		for (int i = 0; i < GetSize(init); i++) {
			if (init[i] != State::S0 && init[i] != State::S1)
				init[i] = State::S0;
		}

		return init;
	}

	void execute(std::vector<std::string> args, RTLIL::Design *design) YS_OVERRIDE
	{
		log_header(design, "Executing ANLOGIC_DETERMINE_INIT pass (determine init value for cells).\n");

		extra_args(args, args.size(), design);

		size_t cnt = 0;
		for (auto module : design->selected_modules())
		{
			for (auto cell : module->selected_cells())
			{
				if (cell->type == "\\EG_LOGIC_DRAM16X4")
				{
					cell->setParam("\\INIT_D0", determine_init(cell->getParam("\\INIT_D0")));
					cell->setParam("\\INIT_D1", determine_init(cell->getParam("\\INIT_D1")));
					cell->setParam("\\INIT_D2", determine_init(cell->getParam("\\INIT_D2")));
					cell->setParam("\\INIT_D3", determine_init(cell->getParam("\\INIT_D3")));
					cnt++;
				}
			}
		}
		log_header(design, "Updated %lu cells with determined init value.\n", cnt);
	}
} AnlogicDetermineInitPass;

PRIVATE_NAMESPACE_END
