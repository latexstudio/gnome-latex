/*
 * This file is part of LaTeXila.
 *
 * Copyright © 2009 Sébastien Wilmet
 *
 * LaTeXila is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LaTeXila is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LaTeXila.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <libintl.h>
#include <gtk/gtk.h>
#include <gtksourceview/gtksourceview.h>

#include "main.h"
#include "config.h"
#include "callbacks.h"
#include "print.h"
#include "symbols.h"

static GtkListStore * get_symbol_store (const struct symbol symbols[]);
static void cb_category_symbols_selected (GtkIconView *icon_view,
		gpointer user_data);
static void cb_symbol_selected (GtkIconView *icon_view, gpointer user_data);

static struct {
	gchar *name;
	gchar *icon;
} categories[] = {
	{N_("Greek"), DATA_DIR "/images/icons/symbol_greek.png"},
	// when we drink too much tequila we walk like this arrow...
	{N_("Arrows"), DATA_DIR "/images/icons/symbol_arrows.png"},
	{N_("Relations"), DATA_DIR "/images/icons/symbol_relations.png"},
	{N_("Operators"), DATA_DIR "/images/icons/symbol_operators.png"},
	{N_("Delimiters"), DATA_DIR "/images/icons/symbol_delimiters.png"},
	{N_("Misc math"), DATA_DIR "/images/icons/symbol_misc_math.png"},
	{N_("Misc text"), DATA_DIR "/images/icons/symbol_misc_text.png"}
};

static struct symbol symbols_greek[] = {
	{DATA_DIR "/images/greek/01.png", "\\alpha", NULL},
	{DATA_DIR "/images/greek/02.png", "\\beta", NULL},
	{DATA_DIR "/images/greek/03.png", "\\gamma", NULL},
	{DATA_DIR "/images/greek/04.png", "\\delta", NULL},
	{DATA_DIR "/images/greek/05.png", "\\epsilon", NULL},
	{DATA_DIR "/images/greek/06.png", "\\varepsilon", NULL},
	{DATA_DIR "/images/greek/07.png", "\\zeta", NULL},
	{DATA_DIR "/images/greek/08.png", "\\eta", NULL},
	{DATA_DIR "/images/greek/09.png", "\\theta", NULL},
	{DATA_DIR "/images/greek/10.png", "\\vartheta", NULL},
	{DATA_DIR "/images/greek/11.png", "\\iota", NULL},
	{DATA_DIR "/images/greek/12.png", "\\kappa", NULL},
	{DATA_DIR "/images/greek/13.png", "\\lambda", NULL},
	{DATA_DIR "/images/greek/14.png", "\\mu", NULL},
	{DATA_DIR "/images/greek/15.png", "\\nu", NULL},
	{DATA_DIR "/images/greek/16.png", "\\xi", NULL},
	{DATA_DIR "/images/greek/17.png", "o", NULL},
	{DATA_DIR "/images/greek/18.png", "\\pi", NULL},
	{DATA_DIR "/images/greek/19.png", "\\varpi", NULL},
	{DATA_DIR "/images/greek/20.png", "\\rho", NULL},
	{DATA_DIR "/images/greek/21.png", "\\varrho", NULL},
	{DATA_DIR "/images/greek/22.png", "\\sigma", NULL},
	{DATA_DIR "/images/greek/23.png", "\\varsigma", NULL},
	{DATA_DIR "/images/greek/24.png", "\\tau", NULL},
	{DATA_DIR "/images/greek/25.png", "\\upsilon", NULL},
	{DATA_DIR "/images/greek/26.png", "\\phi", NULL},
	{DATA_DIR "/images/greek/27.png", "\\varphi", NULL},
	{DATA_DIR "/images/greek/28.png", "\\chi", NULL},
	{DATA_DIR "/images/greek/29.png", "\\psi", NULL},
	{DATA_DIR "/images/greek/30.png", "\\omega", NULL},
	{DATA_DIR "/images/greek/31.png", "A", NULL},
	{DATA_DIR "/images/greek/32.png", "B", NULL},
	{DATA_DIR "/images/greek/33.png", "\\Gamma", NULL},
	{DATA_DIR "/images/greek/34.png", "\\varGamma", "amsmath"},
	{DATA_DIR "/images/greek/35.png", "\\Delta", NULL},
	{DATA_DIR "/images/greek/36.png", "\\varDelta", "amsmath"},
	{DATA_DIR "/images/greek/37.png", "E", NULL},
	{DATA_DIR "/images/greek/38.png", "Z", NULL},
	{DATA_DIR "/images/greek/39.png", "H", NULL},
	{DATA_DIR "/images/greek/40.png", "\\Theta", NULL},
	{DATA_DIR "/images/greek/41.png", "\\varTheta", "amsmath"},
	{DATA_DIR "/images/greek/42.png", "I", NULL},
	{DATA_DIR "/images/greek/43.png", "K", NULL},
	{DATA_DIR "/images/greek/44.png", "\\Lambda", NULL},
	{DATA_DIR "/images/greek/45.png", "\\varLambda", "amsmath"},
	{DATA_DIR "/images/greek/46.png", "M", NULL},
	{DATA_DIR "/images/greek/47.png", "N", NULL},
	{DATA_DIR "/images/greek/48.png", "\\Xi", NULL},
	{DATA_DIR "/images/greek/49.png", "\\varXi", "amsmath"},
	{DATA_DIR "/images/greek/50.png", "O", NULL},
	{DATA_DIR "/images/greek/51.png", "\\Pi", NULL},
	{DATA_DIR "/images/greek/52.png", "\\varPi", "amsmath"},
	{DATA_DIR "/images/greek/53.png", "P", NULL},
	{DATA_DIR "/images/greek/54.png", "\\Sigma", NULL},
	{DATA_DIR "/images/greek/55.png", "\\varSigma", "amsmath"},
	{DATA_DIR "/images/greek/56.png", "T", NULL},
	{DATA_DIR "/images/greek/57.png", "\\Upsilon", NULL},
	{DATA_DIR "/images/greek/58.png", "\\varUpsilon", "amsmath"},
	{DATA_DIR "/images/greek/59.png", "\\Phi", NULL},
	{DATA_DIR "/images/greek/60.png", "\\varPhi", "amsmath"},
	{DATA_DIR "/images/greek/61.png", "X", NULL},
	{DATA_DIR "/images/greek/62.png", "\\Psi", NULL},
	{DATA_DIR "/images/greek/63.png", "\\varPsi", "amsmath"},
	{DATA_DIR "/images/greek/64.png", "\\Omega", NULL},
	{DATA_DIR "/images/greek/65.png", "\\varOmega", "amsmath"},
	{NULL, NULL, NULL}
};

static struct symbol symbols_arrows[] = {
	{DATA_DIR "/images/arrows/01.png", "\\leftarrow", NULL},
	{DATA_DIR "/images/arrows/02.png", "\\leftrightarrow", NULL},
	{DATA_DIR "/images/arrows/03.png", "\\rightarrow", NULL},
	{DATA_DIR "/images/arrows/04.png", "\\mapsto", NULL},
	{DATA_DIR "/images/arrows/05.png", "\\longleftarrow", NULL},
	{DATA_DIR "/images/arrows/06.png", "\\longleftrightarrow", NULL},
	{DATA_DIR "/images/arrows/07.png", "\\longrightarrow", NULL},
	{DATA_DIR "/images/arrows/08.png", "\\longmapsto", NULL},
	{DATA_DIR "/images/arrows/09.png", "\\downarrow", NULL},
	{DATA_DIR "/images/arrows/10.png", "\\updownarrow", NULL},
	{DATA_DIR "/images/arrows/11.png", "\\uparrow", NULL},
	{DATA_DIR "/images/arrows/12.png", "\\nwarrow", NULL},
	{DATA_DIR "/images/arrows/13.png", "\\searrow", NULL},
	{DATA_DIR "/images/arrows/14.png", "\\nearrow", NULL},
	{DATA_DIR "/images/arrows/15.png", "\\swarrow", NULL},
	{DATA_DIR "/images/arrows/16.png", "\\textdownarrow", "textcomp"},
	{DATA_DIR "/images/arrows/17.png", "\\textuparrow", "textcomp"},
	{DATA_DIR "/images/arrows/18.png", "\\textleftarrow", "textcomp"},
	{DATA_DIR "/images/arrows/19.png", "\\textrightarrow", "textcomp"},
	{DATA_DIR "/images/arrows/20.png", "\\nleftarrow", "amssymb"},
	{DATA_DIR "/images/arrows/21.png", "\\nleftrightarrow", "amssymb"},
	{DATA_DIR "/images/arrows/22.png", "\\nrightarrow", "amssymb"},
	{DATA_DIR "/images/arrows/23.png", "\\hookleftarrow", NULL},
	{DATA_DIR "/images/arrows/24.png", "\\hookrightarrow", NULL},
	{DATA_DIR "/images/arrows/25.png", "\\twoheadleftarrow", "amssymb"},
	{DATA_DIR "/images/arrows/26.png", "\\twoheadrightarrow", "amssymb"},
	{DATA_DIR "/images/arrows/27.png", "\\leftarrowtail", "amssymb"},
	{DATA_DIR "/images/arrows/28.png", "\\rightarrowtail", "amssymb"},
	{DATA_DIR "/images/arrows/29.png", "\\Leftarrow", NULL},
	{DATA_DIR "/images/arrows/30.png", "\\Leftrightarrow", NULL},
	{DATA_DIR "/images/arrows/31.png", "\\Rightarrow", NULL},
	{DATA_DIR "/images/arrows/32.png", "\\Longleftarrow", NULL},
	{DATA_DIR "/images/arrows/33.png", "\\Longleftrightarrow", NULL},
	{DATA_DIR "/images/arrows/34.png", "\\Longrightarrow", NULL},
	{DATA_DIR "/images/arrows/35.png", "\\Updownarrow", NULL},
	{DATA_DIR "/images/arrows/36.png", "\\Uparrow", NULL},
	{DATA_DIR "/images/arrows/37.png", "\\Downarrow", NULL},
	{DATA_DIR "/images/arrows/38.png", "\\nLeftarrow", "amssymb"},
	{DATA_DIR "/images/arrows/39.png", "\\nLeftrightarrow", "amssymb"},
	{DATA_DIR "/images/arrows/40.png", "\\nRightarrow", "amssymb"},
	{DATA_DIR "/images/arrows/41.png", "\\leftleftarrows", "amssymb"},
	{DATA_DIR "/images/arrows/42.png", "\\leftrightarrows", "amssymb"},
	{DATA_DIR "/images/arrows/43.png", "\\rightleftarrows", "amssymb"},
	{DATA_DIR "/images/arrows/44.png", "\\rightrightarrows", "amssymb"},
	{DATA_DIR "/images/arrows/45.png", "\\downdownarrows", "amssymb"},
	{DATA_DIR "/images/arrows/46.png", "\\upuparrows", "amssymb"},
	{DATA_DIR "/images/arrows/47.png", "\\circlearrowleft", "amssymb"},
	{DATA_DIR "/images/arrows/48.png", "\\circlearrowright", "amssymb"},
	{DATA_DIR "/images/arrows/49.png", "\\curvearrowleft", "amssymb"},
	{DATA_DIR "/images/arrows/50.png", "\\curvearrowright", "amssymb"},
	{DATA_DIR "/images/arrows/51.png", "\\Lsh", "amssymb"},
	{DATA_DIR "/images/arrows/52.png", "\\Rsh", "amssymb"},
	{DATA_DIR "/images/arrows/53.png", "\\looparrowleft", "amssymb"},
	{DATA_DIR "/images/arrows/54.png", "\\looparrowright", "amssymb"},
	{DATA_DIR "/images/arrows/55.png", "\\dashleftarrow", "amssymb"},
	{DATA_DIR "/images/arrows/56.png", "\\dashrightarrow", "amssymb"},
	{DATA_DIR "/images/arrows/57.png", "\\leftrightsquigarrow", "amssymb"},
	{DATA_DIR "/images/arrows/58.png", "\\rightsquigarrow", "amssymb"},
	{DATA_DIR "/images/arrows/59.png", "\\Lleftarrow", "amssymb"},
	{DATA_DIR "/images/arrows/60.png", "\\leftharpoondown", NULL},
	{DATA_DIR "/images/arrows/61.png", "\\rightharpoondown", NULL},
	{DATA_DIR "/images/arrows/62.png", "\\leftharpoonup", NULL},
	{DATA_DIR "/images/arrows/63.png", "\\rightharpoonup", NULL},
	{DATA_DIR "/images/arrows/64.png", "\\rightleftharpoons", NULL},
	{DATA_DIR "/images/arrows/65.png", "\\leftrightharpoons", "amssymb"},
	{DATA_DIR "/images/arrows/66.png", "\\downharpoonleft", "amssymb"},
	{DATA_DIR "/images/arrows/67.png", "\\upharpoonleft", "amssymb"},
	{DATA_DIR "/images/arrows/68.png", "\\downharpoonright", "amssymb"},
	{DATA_DIR "/images/arrows/69.png", "\\upharpoonright", "amssymb"},
	{NULL, NULL, NULL}
};

static struct symbol symbols_relations[] = {
	{DATA_DIR "/images/relations/001.png", "\\bowtie", NULL},
	{DATA_DIR "/images/relations/002.png", "\\Join", "amssymb"},
	{DATA_DIR "/images/relations/003.png", "\\propto", NULL},
	{DATA_DIR "/images/relations/004.png", "\\varpropto", "amssymb"},
	{DATA_DIR "/images/relations/005.png", "\\multimap", "amssymb"},
	{DATA_DIR "/images/relations/006.png", "\\pitchfork", "amssymb"},
	{DATA_DIR "/images/relations/007.png", "\\therefore", "amssymb"},
	{DATA_DIR "/images/relations/008.png", "\\because", "amssymb"},
	{DATA_DIR "/images/relations/009.png", "=", NULL},
	{DATA_DIR "/images/relations/010.png", "\\neq", NULL},
	{DATA_DIR "/images/relations/011.png", "\\equiv", NULL},
	{DATA_DIR "/images/relations/012.png", "\\approx", NULL},
	{DATA_DIR "/images/relations/013.png", "\\sim", NULL},
	{DATA_DIR "/images/relations/014.png", "\\simeq", NULL},
	{DATA_DIR "/images/relations/015.png", "\\backsimeq", "amssymb"},
	{DATA_DIR "/images/relations/016.png", "\\approxeq", "amssymb"},
	{DATA_DIR "/images/relations/017.png", "\\cong", NULL},
	{DATA_DIR "/images/relations/018.png", "\\ncong", "amssymb"},
	{DATA_DIR "/images/relations/019.png", "\\smile", NULL},
	{DATA_DIR "/images/relations/020.png", "\\frown", NULL},
	{DATA_DIR "/images/relations/021.png", "\\asymp", NULL},
	{DATA_DIR "/images/relations/022.png", "\\smallfrown", "amssymb"},
	{DATA_DIR "/images/relations/023.png", "\\smallsmile", "amssymb"},
	{DATA_DIR "/images/relations/024.png", "\\between", "amssymb"},
	{DATA_DIR "/images/relations/025.png", "\\prec", NULL},
	{DATA_DIR "/images/relations/026.png", "\\succ", NULL},
	{DATA_DIR "/images/relations/027.png", "\\nprec", "amssymb"},
	{DATA_DIR "/images/relations/028.png", "\\nsucc", "amssymb"},
	{DATA_DIR "/images/relations/029.png", "\\preceq", NULL},
	{DATA_DIR "/images/relations/030.png", "\\succeq", NULL},
	{DATA_DIR "/images/relations/031.png", "\\npreceq", "amssymb"},
	{DATA_DIR "/images/relations/032.png", "\\nsucceq", "amssymb"},
	{DATA_DIR "/images/relations/033.png", "\\preccurlyeq", "amssymb"},
	{DATA_DIR "/images/relations/034.png", "\\succcurlyeq", "amssymb"},
	{DATA_DIR "/images/relations/035.png", "\\curlyeqprec", "amssymb"},
	{DATA_DIR "/images/relations/036.png", "\\curlyeqsucc", "amssymb"},
	{DATA_DIR "/images/relations/037.png", "\\precsim", "amssymb"},
	{DATA_DIR "/images/relations/038.png", "\\succsim", "amssymb"},
	{DATA_DIR "/images/relations/039.png", "\\precnsim", "amssymb"},
	{DATA_DIR "/images/relations/040.png", "\\succnsim", "amssymb"},
	{DATA_DIR "/images/relations/041.png", "\\precapprox", "amssymb"},
	{DATA_DIR "/images/relations/042.png", "\\succapprox", "amssymb"},
	{DATA_DIR "/images/relations/043.png", "\\precnapprox", "amssymb"},
	{DATA_DIR "/images/relations/044.png", "\\succnapprox", "amssymb"},
	{DATA_DIR "/images/relations/045.png", "\\perp", NULL},
	{DATA_DIR "/images/relations/046.png", "\\vdash", NULL},
	{DATA_DIR "/images/relations/047.png", "\\dashv", NULL},
	{DATA_DIR "/images/relations/048.png", "\\nvdash", "amssymb"},
	{DATA_DIR "/images/relations/049.png", "\\Vdash", "amssymb"},
	{DATA_DIR "/images/relations/050.png", "\\Vvdash", "amssymb"},
	{DATA_DIR "/images/relations/051.png", "\\models", NULL},
	{DATA_DIR "/images/relations/052.png", "\\vDash", "amssymb"},
	{DATA_DIR "/images/relations/053.png", "\\nvDash", "amssymb"},
	{DATA_DIR "/images/relations/054.png", "\\nVDash", "amssymb"},
	{DATA_DIR "/images/relations/055.png", "\\mid", NULL},
	{DATA_DIR "/images/relations/056.png", "\\nmid", "amssymb"},
	{DATA_DIR "/images/relations/057.png", "\\parallel", NULL},
	{DATA_DIR "/images/relations/058.png", "\\nparallel", "amssymb"},
	{DATA_DIR "/images/relations/059.png", "\\shortmid", "amssymb"},
	{DATA_DIR "/images/relations/060.png", "\\nshortmid", "amssymb"},
	{DATA_DIR "/images/relations/061.png", "\\shortparallel", "amssymb"},
	{DATA_DIR "/images/relations/062.png", "\\nshortparallel", "amssymb"},
	{DATA_DIR "/images/relations/063.png", "<", NULL},
	{DATA_DIR "/images/relations/064.png", ">", NULL},
	{DATA_DIR "/images/relations/065.png", "\\nless", "amssymb"},
	{DATA_DIR "/images/relations/066.png", "\\ngtr", "amssymb"},
	{DATA_DIR "/images/relations/067.png", "\\lessdot", "amssymb"},
	{DATA_DIR "/images/relations/068.png", "\\gtrdot", "amssymb"},
	{DATA_DIR "/images/relations/069.png", "\\ll", NULL},
	{DATA_DIR "/images/relations/070.png", "\\gg", NULL},
	{DATA_DIR "/images/relations/071.png", "\\lll", "amssymb"},
	{DATA_DIR "/images/relations/072.png", "\\ggg", "amssymb"},
	{DATA_DIR "/images/relations/073.png", "\\leq", NULL},
	{DATA_DIR "/images/relations/074.png", "\\geq", NULL},
	{DATA_DIR "/images/relations/075.png", "\\lneq", "amssymb"},
	{DATA_DIR "/images/relations/076.png", "\\gneq", "amssymb"},
	{DATA_DIR "/images/relations/077.png", "\\nleq", "amssymb"},
	{DATA_DIR "/images/relations/078.png", "\\ngeq", "amssymb"},
	{DATA_DIR "/images/relations/079.png", "\\leqq", "amssymb"},
	{DATA_DIR "/images/relations/080.png", "\\geqq", "amssymb"},
	{DATA_DIR "/images/relations/081.png", "\\lneqq", "amssymb"},
	{DATA_DIR "/images/relations/082.png", "\\gneqq", "amssymb"},
	{DATA_DIR "/images/relations/083.png", "\\lvertneqq", "amssymb"},
	{DATA_DIR "/images/relations/084.png", "\\gvertneqq", "amssymb"},
	{DATA_DIR "/images/relations/085.png", "\\nleqq", "amssymb"},
	{DATA_DIR "/images/relations/086.png", "\\ngeqq", "amssymb"},
	{DATA_DIR "/images/relations/087.png", "\\leqslant", "amssymb"},
	{DATA_DIR "/images/relations/088.png", "\\geqslant", "amssymb"},
	{DATA_DIR "/images/relations/089.png", "\\nleqslant", "amssymb"},
	{DATA_DIR "/images/relations/090.png", "\\ngeqslant", "amssymb"},
	{DATA_DIR "/images/relations/091.png", "\\eqslantless", "amssymb"},
	{DATA_DIR "/images/relations/092.png", "\\eqslantgtr", "amssymb"},
	{DATA_DIR "/images/relations/093.png", "\\lessgtr", "amssymb"},
	{DATA_DIR "/images/relations/094.png", "\\gtrless", "amssymb"},
	{DATA_DIR "/images/relations/095.png", "\\lesseqgtr", "amssymb"},
	{DATA_DIR "/images/relations/096.png", "\\gtreqless", "amssymb"},
	{DATA_DIR "/images/relations/097.png", "\\lesseqqgtr", "amssymb"},
	{DATA_DIR "/images/relations/098.png", "\\gtreqqless", "amssymb"},
	{DATA_DIR "/images/relations/099.png", "\\lesssim", "amssymb"},
	{DATA_DIR "/images/relations/100.png", "\\gtrsim", "amssymb"},
	{DATA_DIR "/images/relations/101.png", "\\lnsim", "amssymb"},
	{DATA_DIR "/images/relations/102.png", "\\gnsim", "amssymb"},
	{DATA_DIR "/images/relations/103.png", "\\lessapprox", "amssymb"},
	{DATA_DIR "/images/relations/104.png", "\\gtrapprox", "amssymb"},
	{DATA_DIR "/images/relations/105.png", "\\lnapprox", "amssymb"},
	{DATA_DIR "/images/relations/106.png", "\\gnapprox", "amssymb"},
	{DATA_DIR "/images/relations/107.png", "\\vartriangleleft", "amssymb"},
	{DATA_DIR "/images/relations/108.png", "\\vartriangleright", "amssymb"},
	{DATA_DIR "/images/relations/109.png", "\\ntriangleleft", "amssymb"},
	{DATA_DIR "/images/relations/110.png", "\\ntriangleright", "amssymb"},
	{DATA_DIR "/images/relations/111.png", "\\trianglelefteq", "amssymb"},
	{DATA_DIR "/images/relations/112.png", "\\trianglerighteq", "amssymb"},
	{DATA_DIR "/images/relations/113.png", "\\ntrianglelefteq", "amssymb"},
	{DATA_DIR "/images/relations/114.png", "\\ntrianglerighteq", "amssymb"},
	{DATA_DIR "/images/relations/115.png", "\\blacktriangleleft", "amssymb"},
	{DATA_DIR "/images/relations/116.png", "\\blacktriangleright", "amssymb"},
	{DATA_DIR "/images/relations/117.png", "\\subset", NULL},
	{DATA_DIR "/images/relations/118.png", "\\supset", NULL},
	{DATA_DIR "/images/relations/119.png", "\\subseteq", NULL},
	{DATA_DIR "/images/relations/120.png", "\\supseteq", NULL},
	{DATA_DIR "/images/relations/121.png", "\\subsetneq", "amssymb"},
	{DATA_DIR "/images/relations/122.png", "\\supsetneq", "amssymb"},
	{DATA_DIR "/images/relations/123.png", "\\varsubsetneq", "amssymb"},
	{DATA_DIR "/images/relations/124.png", "\\varsupsetneq", "amssymb"},
	{DATA_DIR "/images/relations/125.png", "\\nsubseteq", "amssymb"},
	{DATA_DIR "/images/relations/126.png", "\\nsupseteq", "amssymb"},
	{DATA_DIR "/images/relations/127.png", "\\subseteqq", "amssymb"},
	{DATA_DIR "/images/relations/128.png", "\\supseteqq", "amssymb"},
	{DATA_DIR "/images/relations/129.png", "\\subsetneqq", "amssymb"},
	{DATA_DIR "/images/relations/130.png", "\\supsetneqq", "amssymb"},
	{DATA_DIR "/images/relations/131.png", "\\nsubseteqq", "amssymb"},
	{DATA_DIR "/images/relations/132.png", "\\nsupseteqq", "amssymb"},
	{DATA_DIR "/images/relations/133.png", "\\backepsilon", "amssymb"},
	{DATA_DIR "/images/relations/134.png", "\\Subset", "amssymb"},
	{DATA_DIR "/images/relations/135.png", "\\Supset", "amssymb"},
	{DATA_DIR "/images/relations/136.png", "\\sqsubset", "amssymb"},
	{DATA_DIR "/images/relations/137.png", "\\sqsupset", "amssymb"},
	{DATA_DIR "/images/relations/138.png", "\\sqsubseteq", NULL},
	{DATA_DIR "/images/relations/139.png", "\\sqsupseteq", NULL},
	{NULL, NULL, NULL}
};

static struct symbol symbols_operators[] = {
	{DATA_DIR "/images/operators/001.png", "\\pm", NULL},
	{DATA_DIR "/images/operators/002.png", "\\mp", NULL},
	{DATA_DIR "/images/operators/003.png", "\\times", NULL},
	{DATA_DIR "/images/operators/004.png", "\\div", NULL},
	{DATA_DIR "/images/operators/005.png", "\\ast", NULL},
	{DATA_DIR "/images/operators/006.png", "\\star", NULL},
	{DATA_DIR "/images/operators/007.png", "\\circ", NULL},
	{DATA_DIR "/images/operators/008.png", "\\bullet", NULL},
	{DATA_DIR "/images/operators/009.png", "\\divideontimes", "amssymb"},
	{DATA_DIR "/images/operators/010.png", "\\ltimes", "amssymb"},
	{DATA_DIR "/images/operators/011.png", "\\rtimes", "amssymb"},
	{DATA_DIR "/images/operators/012.png", "\\cdot", NULL},
	{DATA_DIR "/images/operators/013.png", "\\dotplus", "amssymb"},
	{DATA_DIR "/images/operators/014.png", "\\leftthreetimes", "amssymb"},
	{DATA_DIR "/images/operators/015.png", "\\rightthreetimes", "amssymb"},
	{DATA_DIR "/images/operators/016.png", "\\amalg", NULL},
	{DATA_DIR "/images/operators/017.png", "\\otimes", NULL},
	{DATA_DIR "/images/operators/018.png", "\\oplus", NULL},
	{DATA_DIR "/images/operators/019.png", "\\ominus", NULL},
	{DATA_DIR "/images/operators/020.png", "\\oslash", NULL},
	{DATA_DIR "/images/operators/021.png", "\\odot", NULL},
	{DATA_DIR "/images/operators/022.png", "\\circledcirc", "amssymb"},
	{DATA_DIR "/images/operators/023.png", "\\circleddash", "amssymb"},
	{DATA_DIR "/images/operators/024.png", "\\circledast", "amssymb"},
	{DATA_DIR "/images/operators/025.png", "\\bigcirc", NULL},
	{DATA_DIR "/images/operators/026.png", "\\boxdot", "amssymb"},
	{DATA_DIR "/images/operators/027.png", "\\boxminus", "amssymb"},
	{DATA_DIR "/images/operators/028.png", "\\boxplus", "amssymb"},
	{DATA_DIR "/images/operators/029.png", "\\boxtimes", "amssymb"},
	{DATA_DIR "/images/operators/030.png", "\\diamond", NULL},
	{DATA_DIR "/images/operators/031.png", "\\bigtriangleup", NULL},
	{DATA_DIR "/images/operators/032.png", "\\bigtriangledown", NULL},
	{DATA_DIR "/images/operators/033.png", "\\triangleleft", NULL},
	{DATA_DIR "/images/operators/034.png", "\\triangleright", NULL},
	{DATA_DIR "/images/operators/035.png", "\\lhd", "amssymb"},
	{DATA_DIR "/images/operators/036.png", "\\rhd", "amssymb"},
	{DATA_DIR "/images/operators/037.png", "\\unlhd", "amssymb"},
	{DATA_DIR "/images/operators/038.png", "\\unrhd", "amssymb"},
	{DATA_DIR "/images/operators/039.png", "\\cup", NULL},
	{DATA_DIR "/images/operators/040.png", "\\cap", NULL},
	{DATA_DIR "/images/operators/041.png", "\\uplus", NULL},
	{DATA_DIR "/images/operators/042.png", "\\Cup", "amssymb"},
	{DATA_DIR "/images/operators/043.png", "\\Cap", "amssymb"},
	{DATA_DIR "/images/operators/044.png", "\\wr", NULL},
	{DATA_DIR "/images/operators/045.png", "\\setminus", NULL},
	{DATA_DIR "/images/operators/046.png", "\\smallsetminus", "amssymb"},
	{DATA_DIR "/images/operators/047.png", "\\sqcap", NULL},
	{DATA_DIR "/images/operators/048.png", "\\sqcup", NULL},
	{DATA_DIR "/images/operators/049.png", "\\wedge", NULL},
	{DATA_DIR "/images/operators/050.png", "\\vee", NULL},
	{DATA_DIR "/images/operators/051.png", "\\barwedge", "amssymb"},
	{DATA_DIR "/images/operators/052.png", "\\veebar", "amssymb"},
	{DATA_DIR "/images/operators/053.png", "\\doublebarwedge", "amssymb"},
	{DATA_DIR "/images/operators/054.png", "\\curlywedge", "amssymb"},
	{DATA_DIR "/images/operators/055.png", "\\curlyvee", "amssymb"},
	{DATA_DIR "/images/operators/056.png", "\\dagger", "amssymb"},
	{DATA_DIR "/images/operators/057.png", "\\ddagger", "amssymb"},
	{DATA_DIR "/images/operators/058.png", "\\intercal", "amssymb"},
	{DATA_DIR "/images/operators/059.png", "\\bigcap", NULL},
	{DATA_DIR "/images/operators/060.png", "\\bigcup", NULL},
	{DATA_DIR "/images/operators/061.png", "\\biguplus", NULL},
	{DATA_DIR "/images/operators/062.png", "\\bigsqcup", NULL},
	{DATA_DIR "/images/operators/063.png", "\\prod", NULL},
	{DATA_DIR "/images/operators/064.png", "\\coprod", NULL},
	{DATA_DIR "/images/operators/065.png", "\\bigwedge", NULL},
	{DATA_DIR "/images/operators/066.png", "\\bigvee", NULL},
	{DATA_DIR "/images/operators/067.png", "\\bigodot", NULL},
	{DATA_DIR "/images/operators/068.png", "\\bigoplus", NULL},
	{DATA_DIR "/images/operators/069.png", "\\bigotimes", NULL},
	{DATA_DIR "/images/operators/070.png", "\\sum", NULL},
	{DATA_DIR "/images/operators/071.png", "\\int", NULL},
	{DATA_DIR "/images/operators/072.png", "\\oint", NULL},
	{DATA_DIR "/images/operators/073.png", "\\iint", "amsmath"},
	{DATA_DIR "/images/operators/074.png", "\\iiint", "amsmath"},
	{DATA_DIR "/images/operators/075.png", "\\iiiint", "amsmath"},
	{DATA_DIR "/images/operators/076.png", "\\idotsint", "amsmath"},
	{DATA_DIR "/images/operators/077.png", "\\arccos", NULL},
	{DATA_DIR "/images/operators/078.png", "\\arcsin", NULL},
	{DATA_DIR "/images/operators/079.png", "\\arctan", NULL},
	{DATA_DIR "/images/operators/080.png", "\\arg", NULL},
	{DATA_DIR "/images/operators/081.png", "\\cos", NULL},
	{DATA_DIR "/images/operators/082.png", "\\cosh", NULL},
	{DATA_DIR "/images/operators/083.png", "\\cot", NULL},
	{DATA_DIR "/images/operators/084.png", "\\coth", NULL},
	{DATA_DIR "/images/operators/085.png", "\\csc", NULL},
	{DATA_DIR "/images/operators/086.png", "\\deg", NULL},
	{DATA_DIR "/images/operators/087.png", "\\det", NULL},
	{DATA_DIR "/images/operators/088.png", "\\dim", NULL},
	{DATA_DIR "/images/operators/089.png", "\\exp", NULL},
	{DATA_DIR "/images/operators/090.png", "\\gcd", NULL},
	{DATA_DIR "/images/operators/091.png", "\\hom", NULL},
	{DATA_DIR "/images/operators/092.png", "\\inf", NULL},
	{DATA_DIR "/images/operators/093.png", "\\ker", NULL},
	{DATA_DIR "/images/operators/094.png", "\\lg", NULL},
	{DATA_DIR "/images/operators/095.png", "\\lim", NULL},
	{DATA_DIR "/images/operators/096.png", "\\liminf", NULL},
	{DATA_DIR "/images/operators/097.png", "\\limsup", NULL},
	{DATA_DIR "/images/operators/098.png", "\\ln", NULL},
	{DATA_DIR "/images/operators/099.png", "\\log", NULL},
	{DATA_DIR "/images/operators/100.png", "\\max", NULL},
	{DATA_DIR "/images/operators/101.png", "\\min", NULL},
	{DATA_DIR "/images/operators/102.png", "\\Pr", NULL},
	{DATA_DIR "/images/operators/103.png", "\\projlim", "amsmath"},
	{DATA_DIR "/images/operators/104.png", "\\sec", NULL},
	{DATA_DIR "/images/operators/105.png", "\\sin", NULL},
	{DATA_DIR "/images/operators/106.png", "\\sinh", NULL},
	{DATA_DIR "/images/operators/107.png", "\\sup", NULL},
	{DATA_DIR "/images/operators/108.png", "\\tan", NULL},
	{DATA_DIR "/images/operators/109.png", "\\tanh", NULL},
	{DATA_DIR "/images/operators/110.png", "\\varlimsup", "amsmath"},
	{DATA_DIR "/images/operators/111.png", "\\varliminf", "amsmath"},
	{DATA_DIR "/images/operators/112.png", "\\varinjlim", "amsmath"},
	{DATA_DIR "/images/operators/113.png", "\\varprojlim", "amsmath"},
	{NULL, NULL, NULL}
};

static struct symbol symbols_delimiters[] = {
	{DATA_DIR "/images/delimiters/01.png", "\\downarrow", NULL},
	{DATA_DIR "/images/delimiters/02.png", "\\Downarrow", NULL},
	{DATA_DIR "/images/delimiters/03.png", "[", NULL},
	{DATA_DIR "/images/delimiters/04.png", "]", NULL},
	{DATA_DIR "/images/delimiters/05.png", "\\langle", NULL},
	{DATA_DIR "/images/delimiters/06.png", "\\rangle", NULL},
	{DATA_DIR "/images/delimiters/07.png", "|", NULL},
	{DATA_DIR "/images/delimiters/08.png", "\\|", NULL},
	{DATA_DIR "/images/delimiters/09.png", "\\lceil", NULL},
	{DATA_DIR "/images/delimiters/10.png", "\\rceil", NULL},
	{DATA_DIR "/images/delimiters/11.png", "\\uparrow", NULL},
	{DATA_DIR "/images/delimiters/12.png", "\\Uparrow", NULL},
	{DATA_DIR "/images/delimiters/13.png", "\\lfloor", NULL},
	{DATA_DIR "/images/delimiters/14.png", "\\rfloor", NULL},
	{DATA_DIR "/images/delimiters/15.png", "\\updownarrow", NULL},
	{DATA_DIR "/images/delimiters/16.png", "\\Updownarrow", NULL},
	{DATA_DIR "/images/delimiters/17.png", "(", NULL},
	{DATA_DIR "/images/delimiters/18.png", ")", NULL},
	{DATA_DIR "/images/delimiters/19.png", "\\{", NULL},
	{DATA_DIR "/images/delimiters/20.png", "\\}", NULL},
	{DATA_DIR "/images/delimiters/21.png", "/", NULL},
	{DATA_DIR "/images/delimiters/22.png", "\\backslash", NULL},
	{DATA_DIR "/images/delimiters/23.png", "\\lmoustache", NULL},
	{DATA_DIR "/images/delimiters/24.png", "\\rmoustache", NULL},
	{DATA_DIR "/images/delimiters/25.png", "\\lgroup", NULL},
	{DATA_DIR "/images/delimiters/26.png", "\\rgroup", NULL},
	{DATA_DIR "/images/delimiters/27.png", "\\arrowvert", NULL},
	{DATA_DIR "/images/delimiters/28.png", "\\Arrowvert", NULL},
	{DATA_DIR "/images/delimiters/29.png", "\\bracevert", NULL},
	{DATA_DIR "/images/delimiters/30.png", "\\lvert", "amsmath"},
	{DATA_DIR "/images/delimiters/31.png", "\\rvert", "amsmath"},
	{DATA_DIR "/images/delimiters/32.png", "\\lVert", "amsmath"},
	{DATA_DIR "/images/delimiters/33.png", "\\rVert", "amsmath"},
	{DATA_DIR "/images/delimiters/34.png", "\\ulcorner", "amssymb"},
	{DATA_DIR "/images/delimiters/35.png", "\\urcorner", "amssymb"},
	{DATA_DIR "/images/delimiters/36.png", "\\llcorner", "amssymb"},
	{DATA_DIR "/images/delimiters/37.png", "\\lrcorner", "amssymb"},
	{NULL, NULL, NULL}
};

static struct symbol symbols_misc_math[] = {
	{DATA_DIR "/images/misc-math/01.png", "\\cdotp", NULL},
	{DATA_DIR "/images/misc-math/02.png", "\\colon", NULL},
	{DATA_DIR "/images/misc-math/03.png", "\\ldotp", NULL},
	{DATA_DIR "/images/misc-math/04.png", "\\vdots", NULL},
	{DATA_DIR "/images/misc-math/05.png", "\\cdots", NULL},
	{DATA_DIR "/images/misc-math/06.png", "\\ddots", NULL},
	{DATA_DIR "/images/misc-math/07.png", "\\ldots", NULL},
	{DATA_DIR "/images/misc-math/08.png", "\\neg", NULL},
	{DATA_DIR "/images/misc-math/09.png", "\\infty", NULL},
	{DATA_DIR "/images/misc-math/10.png", "\\prime", NULL},
	{DATA_DIR "/images/misc-math/11.png", "\\backprime", "amssymb"},
	{DATA_DIR "/images/misc-math/12.png", "\\backslash", NULL},
	{DATA_DIR "/images/misc-math/13.png", "\\diagdown", "amssymb"},
	{DATA_DIR "/images/misc-math/14.png", "\\diagup", "amssymb"},
	{DATA_DIR "/images/misc-math/15.png", "\\surd", NULL},
	{DATA_DIR "/images/misc-math/16.png", "\\emptyset", NULL},
	{DATA_DIR "/images/misc-math/17.png", "\\varnothing", "amssymb"},
	{DATA_DIR "/images/misc-math/18.png", "\\sharp", NULL},
	{DATA_DIR "/images/misc-math/19.png", "\\flat", NULL},
	{DATA_DIR "/images/misc-math/20.png", "\\natural", NULL},
	{DATA_DIR "/images/misc-math/21.png", "\\angle", NULL},
	{DATA_DIR "/images/misc-math/22.png", "\\sphericalangle", "amssymb"},
	{DATA_DIR "/images/misc-math/23.png", "\\measuredangle", "amssymb"},
	{DATA_DIR "/images/misc-math/24.png", "\\Box", "amssymb"},
	{DATA_DIR "/images/misc-math/25.png", "\\square", "amssymb"},
	{DATA_DIR "/images/misc-math/26.png", "\\triangle", NULL},
	{DATA_DIR "/images/misc-math/27.png", "\\vartriangle", "amssymb"},
	{DATA_DIR "/images/misc-math/28.png", "\\triangledown", "amssymb"},
	{DATA_DIR "/images/misc-math/29.png", "\\Diamond", "amssymb"},
	{DATA_DIR "/images/misc-math/30.png", "\\diamondsuit", NULL},
	{DATA_DIR "/images/misc-math/31.png", "\\lozenge", "amssymb"},
	{DATA_DIR "/images/misc-math/32.png", "\\heartsuit", NULL},
	{DATA_DIR "/images/misc-math/33.png", "\\blacksquare", "amssymb"},
	{DATA_DIR "/images/misc-math/34.png", "\\blacktriangle", "amssymb"},
	{DATA_DIR "/images/misc-math/35.png", "\\blacktriangledown", "amssymb"},
	{DATA_DIR "/images/misc-math/36.png", "\\blacklozenge", "amssymb"},
	{DATA_DIR "/images/misc-math/37.png", "\\bigstar", "amssymb"},
	{DATA_DIR "/images/misc-math/38.png", "\\spadesuit", NULL},
	{DATA_DIR "/images/misc-math/39.png", "\\clubsuit", NULL},
	{DATA_DIR "/images/misc-math/40.png", "\\forall", NULL},
	{DATA_DIR "/images/misc-math/41.png", "\\exists", NULL},
	{DATA_DIR "/images/misc-math/42.png", "\\nexists", "amssymb"},
	{DATA_DIR "/images/misc-math/43.png", "\\Finv", "amssymb"},
	{DATA_DIR "/images/misc-math/44.png", "\\Game", "amssymb"},
	{DATA_DIR "/images/misc-math/45.png", "\\ni", NULL},
	{DATA_DIR "/images/misc-math/46.png", "\\in", NULL},
	{DATA_DIR "/images/misc-math/47.png", "\\notin", NULL},
	{DATA_DIR "/images/misc-math/48.png", "\\complement", "amssymb"},
	{DATA_DIR "/images/misc-math/set-N.png", "\\mathbb{N}", "amsfonts"},
	{DATA_DIR "/images/misc-math/set-Z.png", "\\mathbb{Z}", "amsfonts"},
	{DATA_DIR "/images/misc-math/set-Q.png", "\\mathbb{Q}", "amsfonts"},
	{DATA_DIR "/images/misc-math/set-I.png", "\\mathbb{I}", "amsfonts"},
	{DATA_DIR "/images/misc-math/set-R.png", "\\mathbb{R}", "amsfonts"},
	{DATA_DIR "/images/misc-math/set-C.png", "\\mathbb{C}", "amsfonts"},
	{DATA_DIR "/images/misc-math/49.png", "\\Im", NULL},
	{DATA_DIR "/images/misc-math/50.png", "\\Re", NULL},
	{DATA_DIR "/images/misc-math/51.png", "\\aleph", NULL},
	{DATA_DIR "/images/misc-math/52.png", "\\wp", NULL},
	{DATA_DIR "/images/misc-math/53.png", "\\hslash", "amssymb"},
	{DATA_DIR "/images/misc-math/54.png", "\\hbar", NULL},
	{DATA_DIR "/images/misc-math/55.png", "\\imath", NULL},
	{DATA_DIR "/images/misc-math/56.png", "\\jmath", NULL},
	{DATA_DIR "/images/misc-math/57.png", "\\Bbbk", "amssymb"},
	{DATA_DIR "/images/misc-math/58.png", "\\ell", NULL},
	{DATA_DIR "/images/misc-math/59.png", "\\circledR", "amssymb"},
	{DATA_DIR "/images/misc-math/60.png", "\\circledS", "amssymb"},
	{DATA_DIR "/images/misc-math/61.png", "\\bot", NULL},
	{DATA_DIR "/images/misc-math/62.png", "\\top", NULL},
	{DATA_DIR "/images/misc-math/63.png", "\\partial", NULL},
	{DATA_DIR "/images/misc-math/64.png", "\\nabla", NULL},
	{DATA_DIR "/images/misc-math/65.png", "\\eth", "amssymb"},
	{DATA_DIR "/images/misc-math/66.png", "\\mho", "amssymb"},
	{DATA_DIR "/images/misc-math/67.png", "\\acute{}", NULL},
	{DATA_DIR "/images/misc-math/68.png", "\\grave{}", NULL},
	{DATA_DIR "/images/misc-math/69.png", "\\check{}", NULL},
	{DATA_DIR "/images/misc-math/70.png", "\\hat{}", NULL},
	{DATA_DIR "/images/misc-math/71.png", "\\tilde{}", NULL},
	{DATA_DIR "/images/misc-math/72.png", "\\bar{}", NULL},
	{DATA_DIR "/images/misc-math/73.png", "\\vec{}", NULL},
	{DATA_DIR "/images/misc-math/74.png", "\\breve{}", NULL},
	{DATA_DIR "/images/misc-math/75.png", "\\dot{}", NULL},
	{DATA_DIR "/images/misc-math/76.png", "\\ddot{}", NULL},
	{DATA_DIR "/images/misc-math/77.png", "\\dddot{}", "amsmath"},
	{DATA_DIR "/images/misc-math/78.png", "\\ddddot{}", "amsmath"},
	{DATA_DIR "/images/misc-math/79.png", "\\mathring{}", NULL},
	{DATA_DIR "/images/misc-math/80.png", "\\widetilde{}", NULL},
	{DATA_DIR "/images/misc-math/81.png", "\\widehat{}", NULL},
	{DATA_DIR "/images/misc-math/82.png", "\\overleftarrow{}", NULL},
	{DATA_DIR "/images/misc-math/83.png", "\\overrightarrow{}", NULL},
	{DATA_DIR "/images/misc-math/84.png", "\\overline{}", NULL},
	{DATA_DIR "/images/misc-math/85.png", "\\underline{}", NULL},
	{DATA_DIR "/images/misc-math/86.png", "\\overbrace{}", NULL},
	{DATA_DIR "/images/misc-math/87.png", "\\underbrace{}", NULL},
	{DATA_DIR "/images/misc-math/88.png", "\\overleftrightarrow{}", "amsmath"},
	{DATA_DIR "/images/misc-math/89.png", "\\underleftrightarrow{}", "amsmath"},
	{DATA_DIR "/images/misc-math/90.png", "\\underleftarrow{}", "amsmath"},
	{DATA_DIR "/images/misc-math/91.png", "\\underrightarrow{}", "amsmath"},
	{DATA_DIR "/images/misc-math/92.png", "\\xleftarrow{}", "amsmath"},
	{DATA_DIR "/images/misc-math/93.png", "\\xrightarrow{}", "amsmath"},
	{DATA_DIR "/images/misc-math/94.png", "\\stackrel{}{}", NULL},
	{DATA_DIR "/images/misc-math/95.png", "\\sqrt{}", NULL},
	{DATA_DIR "/images/misc-math/96.png", "f'", NULL},
	{DATA_DIR "/images/misc-math/97.png", "f''", NULL},
	{NULL, NULL, NULL}
};

static struct symbol symbols_misc_text[] = {
	{DATA_DIR "/images/misc-text/001.png", "\\dots", NULL},
	{DATA_DIR "/images/misc-text/002.png", "\\texttildelow", "textcomp"},
	{DATA_DIR "/images/misc-text/003.png", "\\textasciicircum", NULL},
	{DATA_DIR "/images/misc-text/004.png", "\\textasciimacron", "textcomp"},
	{DATA_DIR "/images/misc-text/005.png", "\\textasciiacute", "textcomp"},
	{DATA_DIR "/images/misc-text/006.png", "\\textasciidieresis", "textcomp"},
	{DATA_DIR "/images/misc-text/007.png", "\\textasciitilde", NULL},
	{DATA_DIR "/images/misc-text/008.png", "\\textasciigrave", "textcomp"},
	{DATA_DIR "/images/misc-text/009.png", "\\textasciibreve", "textcomp"},
	{DATA_DIR "/images/misc-text/010.png", "\\textasciicaron", "textcomp"},
	{DATA_DIR "/images/misc-text/011.png", "\\textacutedbl", "textcomp"},
	{DATA_DIR "/images/misc-text/012.png", "\\textgravedbl", "textcomp"},
	{DATA_DIR "/images/misc-text/013.png", "\\textquotedblleft", NULL},
	{DATA_DIR "/images/misc-text/014.png", "\\textquotedblright", NULL},
	{DATA_DIR "/images/misc-text/015.png", "\\textquoteleft", NULL},
	{DATA_DIR "/images/misc-text/016.png", "\\textquoteright", NULL},
	{DATA_DIR "/images/misc-text/017.png", "\\textquotestraightbase", "textcomp"},
	{DATA_DIR "/images/misc-text/018.png", "\\textquotestraightdblbase", "textcomp"},
	{DATA_DIR "/images/misc-text/019.png", "\\textquotesingle", "textcomp"},
	{DATA_DIR "/images/misc-text/020.png", "\\textdblhyphen", "textcomp"},
	{DATA_DIR "/images/misc-text/021.png", "\\textdblhyphenchar", "textcomp"},
	{DATA_DIR "/images/misc-text/022.png", "\\textasteriskcentered", NULL},
	{DATA_DIR "/images/misc-text/023.png", "\\textperiodcentered", NULL},
	{DATA_DIR "/images/misc-text/024.png", "\\textquestiondown", NULL},
	{DATA_DIR "/images/misc-text/025.png", "\\textinterrobang", "textcomp"},
	{DATA_DIR "/images/misc-text/026.png", "\\textinterrobangdown", "textcomp"},
	{DATA_DIR "/images/misc-text/027.png", "\\textexclamdown", NULL},
	{DATA_DIR "/images/misc-text/028.png", "\\texttwelveudash", "textcomp"},
	{DATA_DIR "/images/misc-text/029.png", "\\textemdash", NULL},
	{DATA_DIR "/images/misc-text/030.png", "\\textendash", NULL},
	{DATA_DIR "/images/misc-text/031.png", "\\textthreequartersemdash", "textcomp"},
	{DATA_DIR "/images/misc-text/032.png", "\\textvisiblespace", NULL},
	{DATA_DIR "/images/misc-text/033.png", "\\_", NULL},
	{DATA_DIR "/images/misc-text/034.png", "\\textcurrency", "textcomp"},
	{DATA_DIR "/images/misc-text/035.png", "\\textbaht", "textcomp"},
	{DATA_DIR "/images/misc-text/036.png", "\\textguarani", "textcomp"},
	{DATA_DIR "/images/misc-text/037.png", "\\textwon", "textcomp"},
	{DATA_DIR "/images/misc-text/038.png", "\\textcent", "textcomp"},
	{DATA_DIR "/images/misc-text/039.png", "\\textcentoldstyle", "textcomp"},
	{DATA_DIR "/images/misc-text/040.png", "\\textdollar", NULL},
	{DATA_DIR "/images/misc-text/041.png", "\\textdollaroldstyle", "textcomp"},
	{DATA_DIR "/images/misc-text/042.png", "\\textlira", "textcomp"},
	{DATA_DIR "/images/misc-text/043.png", "\\textyen", "textcomp"},
	{DATA_DIR "/images/misc-text/044.png", "\\textdong", "textcomp"},
	{DATA_DIR "/images/misc-text/045.png", "\\textnaira", "textcomp"},
	{DATA_DIR "/images/misc-text/046.png", "\\textcolonmonetary", "textcomp"},
	{DATA_DIR "/images/misc-text/047.png", "\\textpeso", "textcomp"},
	{DATA_DIR "/images/misc-text/048.png", "\\pounds", NULL},
	{DATA_DIR "/images/misc-text/049.png", "\\textflorin", "textcomp"},
	{DATA_DIR "/images/misc-text/050.png", "\\texteuro", "textcomp"},
	{DATA_DIR "/images/misc-text/051.png", "\\geneuro", "eurosym"},
	{DATA_DIR "/images/misc-text/052.png", "\\geneuronarrow", "eurosym"},
	{DATA_DIR "/images/misc-text/053.png", "\\geneurowide", "eurosym"},
	{DATA_DIR "/images/misc-text/054.png", "\\officialeuro", "eurosym"},
	{DATA_DIR "/images/misc-text/055.png", "\\textcircled{a}", NULL},
	{DATA_DIR "/images/misc-text/056.png", "\\textcopyright", NULL},
	{DATA_DIR "/images/misc-text/057.png", "\\textcopyleft", "textcomp"},
	{DATA_DIR "/images/misc-text/058.png", "\\textregistered", NULL},
	{DATA_DIR "/images/misc-text/059.png", "\\texttrademark", NULL},
	{DATA_DIR "/images/misc-text/060.png", "\\textservicemark", "textcomp"},
	{DATA_DIR "/images/misc-text/061.png", "\\oldstylenums{0}", NULL},
	{DATA_DIR "/images/misc-text/062.png", "\\oldstylenums{1}", NULL},
	{DATA_DIR "/images/misc-text/063.png", "\\oldstylenums{2}", NULL},
	{DATA_DIR "/images/misc-text/064.png", "\\oldstylenums{3}", NULL},
	{DATA_DIR "/images/misc-text/065.png", "\\oldstylenums{4}", NULL},
	{DATA_DIR "/images/misc-text/066.png", "\\oldstylenums{5}", NULL},
	{DATA_DIR "/images/misc-text/067.png", "\\oldstylenums{6}", NULL},
	{DATA_DIR "/images/misc-text/068.png", "\\oldstylenums{7}", NULL},
	{DATA_DIR "/images/misc-text/069.png", "\\oldstylenums{8}", NULL},
	{DATA_DIR "/images/misc-text/070.png", "\\oldstylenums{9}", NULL},
	{DATA_DIR "/images/misc-text/071.png", "\\textonehalf", "textcomp"},
	{DATA_DIR "/images/misc-text/072.png", "\\textonequarter", "textcomp"},
	{DATA_DIR "/images/misc-text/073.png", "\\textthreequarters", "textcomp"},
	{DATA_DIR "/images/misc-text/074.png", "\\textonesuperior", "textcomp"},
	{DATA_DIR "/images/misc-text/075.png", "\\texttwosuperior", "textcomp"},
	{DATA_DIR "/images/misc-text/076.png", "\\textthreesuperior", "textcomp"},
	{DATA_DIR "/images/misc-text/077.png", "\\textnumero", "textcomp"},
	{DATA_DIR "/images/misc-text/078.png", "\\textpertenthousand", "textcomp"},
	{DATA_DIR "/images/misc-text/079.png", "\\textperthousand", "textcomp"},
	{DATA_DIR "/images/misc-text/080.png", "\\textdiscount", "textcomp"},
	{DATA_DIR "/images/misc-text/081.png", "\\textblank", "textcomp"},
	{DATA_DIR "/images/misc-text/082.png", "\\textrecipe", "textcomp"},
	{DATA_DIR "/images/misc-text/083.png", "\\textestimated", "textcomp"},
	{DATA_DIR "/images/misc-text/084.png", "\\textreferencemark", "textcomp"},
	{DATA_DIR "/images/misc-text/085.png", "\\textmusicalnote", "textcomp"},
	{DATA_DIR "/images/misc-text/086.png", "\\dag", NULL},
	{DATA_DIR "/images/misc-text/087.png", "\\ddag", NULL},
	{DATA_DIR "/images/misc-text/088.png", "\\S", NULL},
	{DATA_DIR "/images/misc-text/089.png", "\\$", NULL},
	{DATA_DIR "/images/misc-text/090.png", "\\textpilcrow", "textcomp"},
	{DATA_DIR "/images/misc-text/091.png", "\\Cutleft", "marvosym"},
	{DATA_DIR "/images/misc-text/092.png", "\\Cutright", "marvosym"},
	{DATA_DIR "/images/misc-text/093.png", "\\Leftscissors", "marvosym"},
	{DATA_DIR "/images/misc-text/094.png", "\\Cutline", "marvosym"},
	{DATA_DIR "/images/misc-text/095.png", "\\Kutline", "marvosym"},
	{DATA_DIR "/images/misc-text/096.png", "\\Rightscissors", "marvosym"},
	{DATA_DIR "/images/misc-text/097.png", "\\CheckedBox", "wasysym"},
	{DATA_DIR "/images/misc-text/098.png", "\\Square", "wasysym"},
	{DATA_DIR "/images/misc-text/099.png", "\\XBox", "wasysym"},
	{DATA_DIR "/images/misc-text/100.png", "\\textbigcircle", "textcomp"},
	{DATA_DIR "/images/misc-text/101.png", "\\textopenbullet", "textcomp"},
	{DATA_DIR "/images/misc-text/102.png", "\\textbullet", NULL},
	{DATA_DIR "/images/misc-text/103.png", "\\checkmark", "amssymb"},
	{DATA_DIR "/images/misc-text/104.png", "\\maltese", "amssymb"},
	{DATA_DIR "/images/misc-text/105.png", "\\textordmasculine", "textcomp"},
	{DATA_DIR "/images/misc-text/106.png", "\\textordfeminine", "textcomp"},
	{DATA_DIR "/images/misc-text/107.png", "\\textborn", "textcomp"},
	{DATA_DIR "/images/misc-text/108.png", "\\textdivorced", "textcomp"},
	{DATA_DIR "/images/misc-text/109.png", "\\textdied", "textcomp"},
	{DATA_DIR "/images/misc-text/110.png", "\\textmarried", "textcomp"},
	{DATA_DIR "/images/misc-text/111.png", "\\textleaf", "textcomp"},
	{DATA_DIR "/images/misc-text/112.png", "\\textcelsius", "textcomp"},
	{DATA_DIR "/images/misc-text/113.png", "\\textdegree", "textcomp"},
	{DATA_DIR "/images/misc-text/114.png", "\\textmho", "textcomp"},
	{DATA_DIR "/images/misc-text/115.png", "\\textohm", "textcomp"},
	{DATA_DIR "/images/misc-text/116.png", "\\textbackslash", NULL},
	{DATA_DIR "/images/misc-text/117.png", "\\textbar", NULL},
	{DATA_DIR "/images/misc-text/118.png", "\\textbrokenbar", "textcomp"},
	{DATA_DIR "/images/misc-text/119.png", "\\textbardbl", NULL},
	{DATA_DIR "/images/misc-text/120.png", "\\textfractionsolidus", "textcomp"},
	{DATA_DIR "/images/misc-text/121.png", "\\textlangle", "textcomp"},
	{DATA_DIR "/images/misc-text/122.png", "\\textlnot", "textcomp"},
	{DATA_DIR "/images/misc-text/123.png", "\\textminus", "textcomp"},
	{DATA_DIR "/images/misc-text/124.png", "\\textrangle", "textcomp"},
	{DATA_DIR "/images/misc-text/125.png", "\\textlbrackdbl", "textcomp"},
	{DATA_DIR "/images/misc-text/126.png", "\\textrbrackdbl", "textcomp"},
	{DATA_DIR "/images/misc-text/127.png", "\\textmu", "textcomp"},
	{DATA_DIR "/images/misc-text/128.png", "\\textpm", "textcomp"},
	{DATA_DIR "/images/misc-text/129.png", "\\textlquill", "textcomp"},
	{DATA_DIR "/images/misc-text/130.png", "\\textrquill", "textcomp"},
	{DATA_DIR "/images/misc-text/131.png", "\\textless", NULL},
	{DATA_DIR "/images/misc-text/132.png", "\\textgreater", NULL},
	{DATA_DIR "/images/misc-text/133.png", "\\textsurd", "textcomp"},
	{DATA_DIR "/images/misc-text/134.png", "\\texttimes", "textcomp"},
	{DATA_DIR "/images/misc-text/135.png", "\\textdiv", "textcomp"},
	{NULL, NULL, NULL}
};

// symbols must be NULL-terminated
static GtkListStore *
get_symbol_store (const struct symbol symbols[])
{
	GError *error = NULL;
	GtkListStore *symbol_store = gtk_list_store_new (N_COLUMNS_SYMBOL,
			GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING);

	for (gint i = 0 ; symbols[i].filename != NULL ; i++)
	{
		struct symbol current_symbol = symbols[i];

		GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file (current_symbol.filename,
				&error);
		if (error != NULL)
		{
			print_warning ("impossible to load the symbol: %s", error->message);
			g_error_free (error);
			error = NULL;
			continue;
		}

		// some characters ('<' for example) generate errors for the tooltip,
		// so we must escape it
		gchar *latex_command_escaped = g_markup_escape_text (
				current_symbol.latex_command, -1);
		gchar *tooltip;
		if (current_symbol.package_required != NULL)
		{
			tooltip = g_strdup_printf ("%s (package %s)",
					latex_command_escaped,
					current_symbol.package_required);
			g_free (latex_command_escaped);
		}
		else
			tooltip = latex_command_escaped;


		GtkTreeIter iter;
		gtk_list_store_append (symbol_store, &iter);
		gtk_list_store_set (symbol_store, &iter,
				COLUMN_SYMBOL_PIXBUF, pixbuf,
				COLUMN_SYMBOL_COMMAND, current_symbol.latex_command,
				COLUMN_SYMBOL_TOOLTIP, tooltip,
				-1);
		g_free (tooltip);
		g_object_unref (pixbuf);
	}

	return symbol_store;
}

void
init_symbols (void)
{
	GError *error = NULL;

	// store the categories
	GtkListStore *categories_store = gtk_list_store_new (N_COLUMNS_CAT,
			GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_INT);

	gint nb_categories = G_N_ELEMENTS (categories);
	for (gint i = 0 ; i < nb_categories ; i++)
	{
		GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file (categories[i].icon,
				&error);
		if (error != NULL)
		{
			print_warning ("impossible to load the symbol: %s", error->message);
			g_error_free (error);
			error = NULL;
			continue;
		}

		GtkTreeIter iter;
		gtk_list_store_append (categories_store, &iter);
		gtk_list_store_set (categories_store, &iter,
				COLUMN_CAT_ICON, pixbuf,
				COLUMN_CAT_NAME, _(categories[i].name),
				COLUMN_CAT_NUM, i,
				-1);
	}

	// show the categories
	GtkWidget *categories_view = gtk_icon_view_new_with_model (
			GTK_TREE_MODEL (categories_store));
	gtk_icon_view_set_pixbuf_column (GTK_ICON_VIEW (categories_view),
			COLUMN_CAT_ICON);
	gtk_icon_view_set_text_column (GTK_ICON_VIEW (categories_view),
			COLUMN_CAT_NAME);
	gtk_icon_view_set_selection_mode (GTK_ICON_VIEW (categories_view),
			GTK_SELECTION_SINGLE);

	gtk_icon_view_set_orientation (GTK_ICON_VIEW (categories_view),
			GTK_ORIENTATION_HORIZONTAL);
	//gtk_icon_view_set_columns (GTK_ICON_VIEW (categories_view), -1);

	gtk_icon_view_set_spacing (GTK_ICON_VIEW (categories_view), 5);
	gtk_icon_view_set_row_spacing (GTK_ICON_VIEW (categories_view), 0);
	gtk_icon_view_set_column_spacing (GTK_ICON_VIEW (categories_view), 0);

	g_signal_connect (G_OBJECT (categories_view), "selection-changed",
			G_CALLBACK (cb_category_symbols_selected), NULL);

	gtk_box_pack_start (GTK_BOX (latexila.symbols.vbox), categories_view, FALSE, FALSE, 0);

	// store all the symbols
	// Attention, we must save the GtkListStore in latexila.symbols.list_stores
	// in the same order than the structure "categories" above
	latexila.symbols.list_stores[0] = get_symbol_store (symbols_greek);
	latexila.symbols.list_stores[1] = get_symbol_store (symbols_arrows);
	latexila.symbols.list_stores[2] = get_symbol_store (symbols_relations);
	latexila.symbols.list_stores[3] = get_symbol_store (symbols_operators);
	latexila.symbols.list_stores[4] = get_symbol_store (symbols_delimiters);
	latexila.symbols.list_stores[5] = get_symbol_store (symbols_misc_math);
	latexila.symbols.list_stores[6] = get_symbol_store (symbols_misc_text);

	// show the symbols
	GtkWidget *symbol_view = gtk_icon_view_new_with_model (
			GTK_TREE_MODEL (latexila.symbols.list_stores[0]));
	latexila.symbols.icon_view = GTK_ICON_VIEW (symbol_view);
	gtk_icon_view_set_pixbuf_column (latexila.symbols.icon_view,
			COLUMN_SYMBOL_PIXBUF);
	gtk_icon_view_set_tooltip_column (latexila.symbols.icon_view,
			COLUMN_SYMBOL_TOOLTIP);
	gtk_icon_view_set_selection_mode (latexila.symbols.icon_view,
			GTK_SELECTION_SINGLE);
	gtk_icon_view_set_spacing (latexila.symbols.icon_view, 0);
	gtk_icon_view_set_row_spacing (latexila.symbols.icon_view, 0);
	gtk_icon_view_set_column_spacing (latexila.symbols.icon_view, 0);

	g_signal_connect (G_OBJECT (symbol_view), "selection-changed",
			G_CALLBACK (cb_symbol_selected), NULL);

	// with a scrollbar
	GtkWidget *scrollbar = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollbar),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add (GTK_CONTAINER (scrollbar), symbol_view);
	gtk_box_pack_start (GTK_BOX (latexila.symbols.vbox), scrollbar, TRUE, TRUE, 0);
}

static void
cb_category_symbols_selected (GtkIconView *icon_view, gpointer user_data)
{
	GList *selected_items = gtk_icon_view_get_selected_items (icon_view);
	GtkTreePath *path = g_list_nth_data (selected_items, 0);
	GtkTreeModel *model = gtk_icon_view_get_model (icon_view);
	GtkTreeIter iter;

	if (path != NULL && gtk_tree_model_get_iter (model, &iter, path))
	{
		gint num;
		gtk_tree_model_get (model, &iter, COLUMN_CAT_NUM, &num, -1);

		// change the model
		gtk_icon_view_set_model (latexila.symbols.icon_view,
				GTK_TREE_MODEL (latexila.symbols.list_stores[num]));

		// TODO scroll to the start
		/* this doesn't work...
		GtkTreePath *first_path = gtk_icon_view_get_path_at_pos (
				latexila.symbols.icon_view, 0, 0);
		if (first_path != NULL)
		{
			gtk_icon_view_scroll_to_path (latexila.symbols.icon_view, first_path,
					TRUE, 0.0, 0.0);
		}
		*/
	}

	// free the GList
	g_list_foreach (selected_items, (GFunc) gtk_tree_path_free, NULL);
	g_list_free (selected_items);
}

static void
cb_symbol_selected (GtkIconView *icon_view, gpointer user_data)
{
	if (latexila.active_doc == NULL)
	{
		gtk_icon_view_unselect_all (icon_view);
		return;
	}

	GList *selected_items = gtk_icon_view_get_selected_items (icon_view);
	// unselect the symbol, so the user can insert several times the same symbol
	gtk_icon_view_unselect_all (icon_view);
	GtkTreePath *path = g_list_nth_data (selected_items, 0);
	GtkTreeModel *model = gtk_icon_view_get_model (icon_view);
	GtkTreeIter iter;

	if (path != NULL && gtk_tree_model_get_iter (model, &iter, path))
	{
		gchar *latex_command;
		gtk_tree_model_get (model, &iter,
				COLUMN_SYMBOL_COMMAND, &latex_command,
				-1);

		// insert the symbol in the current document
		GtkTextBuffer *buffer =
			GTK_TEXT_BUFFER (latexila.active_doc->source_buffer);
		gtk_text_buffer_begin_user_action (buffer);
		gtk_text_buffer_insert_at_cursor (
				GTK_TEXT_BUFFER (latexila.active_doc->source_buffer),
				latex_command, -1);
		gtk_text_buffer_insert_at_cursor (
				GTK_TEXT_BUFFER (latexila.active_doc->source_buffer), " ", -1);
		gtk_text_buffer_end_user_action (buffer);

		gtk_widget_grab_focus (latexila.active_doc->source_view);

		g_free (latex_command);
	}

	// free the GList
	g_list_foreach (selected_items, (GFunc) gtk_tree_path_free, NULL);
	g_list_free (selected_items);
}
