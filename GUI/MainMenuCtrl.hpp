#pragma once

auto& p = menu.push_back("Project");
p.append("Load", std::bind(&loadProject, fm));
p.append("Save", std::bind(&saveProject, fm));
p.append("Export", std::bind(&exportProject, fm));