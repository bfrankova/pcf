/**
 * Copyright (C) 2012 Libor Polčák <ipolcak@fit.vutbr.cz>
 * 
 * This file is part of pcf - PC fingerprinter.
 *
 * Pcf is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Pcf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with pcf. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _GNUPLOT_GRAPH_H
#define _GNUPLOT_GRAPH_H

#include "TimeSegment.h"
#include "AnalysisInfo.h"
#include "Observer.h"

class gnuplot_graph: public Observer<const AnalysisInfo>
{
  public:
    virtual void Notify(const AnalysisInfo& changed_skew);

  private:
    void generate_graph(const AnalysisInfo& changed_skew);
};

#endif
