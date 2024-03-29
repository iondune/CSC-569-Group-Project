# vim: ft=make
.PHONY: ACM_proc._graphics
ACM_proc.aux ACM_proc.aux.make ACM_proc.d ACM_proc.pdf: $(call path-norm,/usr/local/texlive/2012/texmf-dist/tex/generic/oberdiek/etexcmds.sty)
ACM_proc.aux ACM_proc.aux.make ACM_proc.d ACM_proc.pdf: $(call path-norm,/usr/local/texlive/2012/texmf-dist/tex/generic/oberdiek/ifluatex.sty)
ACM_proc.aux ACM_proc.aux.make ACM_proc.d ACM_proc.pdf: $(call path-norm,/usr/local/texlive/2012/texmf-dist/tex/generic/oberdiek/ifpdf.sty)
ACM_proc.aux ACM_proc.aux.make ACM_proc.d ACM_proc.pdf: $(call path-norm,/usr/local/texlive/2012/texmf-dist/tex/generic/oberdiek/infwarerr.sty)
ACM_proc.aux ACM_proc.aux.make ACM_proc.d ACM_proc.pdf: $(call path-norm,/usr/local/texlive/2012/texmf-dist/tex/generic/oberdiek/kvdefinekeys.sty)
ACM_proc.aux ACM_proc.aux.make ACM_proc.d ACM_proc.pdf: $(call path-norm,/usr/local/texlive/2012/texmf-dist/tex/generic/oberdiek/kvsetkeys.sty)
ACM_proc.aux ACM_proc.aux.make ACM_proc.d ACM_proc.pdf: $(call path-norm,/usr/local/texlive/2012/texmf-dist/tex/generic/oberdiek/ltxcmds.sty)
ACM_proc.aux ACM_proc.aux.make ACM_proc.d ACM_proc.pdf: $(call path-norm,/usr/local/texlive/2012/texmf-dist/tex/generic/oberdiek/pdftexcmds.sty)
ACM_proc.aux ACM_proc.aux.make ACM_proc.d ACM_proc.pdf: $(call path-norm,/usr/local/texlive/2012/texmf-dist/tex/latex/amsfonts/amsfonts.sty)
ACM_proc.aux ACM_proc.aux.make ACM_proc.d ACM_proc.pdf: $(call path-norm,/usr/local/texlive/2012/texmf-dist/tex/latex/amsfonts/amssymb.sty)
ACM_proc.aux ACM_proc.aux.make ACM_proc.d ACM_proc.pdf: $(call path-norm,/usr/local/texlive/2012/texmf-dist/tex/latex/amsmath/amsbsy.sty)
ACM_proc.aux ACM_proc.aux.make ACM_proc.d ACM_proc.pdf: $(call path-norm,/usr/local/texlive/2012/texmf-dist/tex/latex/amsmath/amsgen.sty)
ACM_proc.aux ACM_proc.aux.make ACM_proc.d ACM_proc.pdf: $(call path-norm,/usr/local/texlive/2012/texmf-dist/tex/latex/amsmath/amsmath.sty)
ACM_proc.aux ACM_proc.aux.make ACM_proc.d ACM_proc.pdf: $(call path-norm,/usr/local/texlive/2012/texmf-dist/tex/latex/amsmath/amsopn.sty)
ACM_proc.aux ACM_proc.aux.make ACM_proc.d ACM_proc.pdf: $(call path-norm,/usr/local/texlive/2012/texmf-dist/tex/latex/amsmath/amstext.sty)
ACM_proc.aux ACM_proc.aux.make ACM_proc.d ACM_proc.pdf: $(call path-norm,/usr/local/texlive/2012/texmf-dist/tex/latex/base/fontenc.sty)
ACM_proc.aux ACM_proc.aux.make ACM_proc.d ACM_proc.pdf: $(call path-norm,/usr/local/texlive/2012/texmf-dist/tex/latex/base/latexsym.sty)
ACM_proc.aux ACM_proc.aux.make ACM_proc.d ACM_proc.pdf: $(call path-norm,/usr/local/texlive/2012/texmf-dist/tex/latex/float/float.sty)
ACM_proc.aux ACM_proc.aux.make ACM_proc.d ACM_proc.pdf: $(call path-norm,/usr/local/texlive/2012/texmf-dist/tex/latex/graphics/epsfig.sty)
ACM_proc.aux ACM_proc.aux.make ACM_proc.d ACM_proc.pdf: $(call path-norm,/usr/local/texlive/2012/texmf-dist/tex/latex/graphics/graphics.sty)
ACM_proc.aux ACM_proc.aux.make ACM_proc.d ACM_proc.pdf: $(call path-norm,/usr/local/texlive/2012/texmf-dist/tex/latex/graphics/graphicx.sty)
ACM_proc.aux ACM_proc.aux.make ACM_proc.d ACM_proc.pdf: $(call path-norm,/usr/local/texlive/2012/texmf-dist/tex/latex/graphics/keyval.sty)
ACM_proc.aux ACM_proc.aux.make ACM_proc.d ACM_proc.pdf: $(call path-norm,/usr/local/texlive/2012/texmf-dist/tex/latex/graphics/trig.sty)
ACM_proc.aux ACM_proc.aux.make ACM_proc.d ACM_proc.pdf: $(call path-norm,/usr/local/texlive/2012/texmf-dist/tex/latex/oberdiek/epstopdf-base.sty)
ACM_proc.aux ACM_proc.aux.make ACM_proc.d ACM_proc.pdf: $(call path-norm,/usr/local/texlive/2012/texmf-dist/tex/latex/oberdiek/grfext.sty)
ACM_proc.aux ACM_proc.aux.make ACM_proc.d ACM_proc.pdf: $(call path-norm,/usr/local/texlive/2012/texmf-dist/tex/latex/oberdiek/kvoptions.sty)
ACM_proc.aux ACM_proc.aux.make ACM_proc.d ACM_proc.pdf: $(call path-norm,ACM_proc.tex)
ACM_proc.aux ACM_proc.aux.make ACM_proc.d ACM_proc.pdf: $(call path-norm,acm_proc_article-sp.cls)
.SECONDEXPANSION:
-include Pics/MrMPI.png.gpi.d
ACM_proc.d: $$(call graphics-source,Pics/MrMPI.png)
ACM_proc.pdf ACM_proc._graphics: $$(call graphics-target,Pics/MrMPI.png)
-include Pics/MPI.png.gpi.d
ACM_proc.d: $$(call graphics-source,Pics/MPI.png)
ACM_proc.pdf ACM_proc._graphics: $$(call graphics-target,Pics/MPI.png)
-include Pics/10000.png.gpi.d
ACM_proc.d: $$(call graphics-source,Pics/10000.png)
ACM_proc.pdf ACM_proc._graphics: $$(call graphics-target,Pics/10000.png)
-include Pics/100000.png.gpi.d
ACM_proc.d: $$(call graphics-source,Pics/100000.png)
ACM_proc.pdf ACM_proc._graphics: $$(call graphics-target,Pics/100000.png)
-include Pics/1000000.png.gpi.d
ACM_proc.d: $$(call graphics-source,Pics/1000000.png)
ACM_proc.pdf ACM_proc._graphics: $$(call graphics-target,Pics/1000000.png)
-include Pics/10000000.png.gpi.d
ACM_proc.d: $$(call graphics-source,Pics/10000000.png)
ACM_proc.pdf ACM_proc._graphics: $$(call graphics-target,Pics/10000000.png)
-include Pics/10000c.png.gpi.d
ACM_proc.d: $$(call graphics-source,Pics/10000c.png)
ACM_proc.pdf ACM_proc._graphics: $$(call graphics-target,Pics/10000c.png)
-include Pics/100000c.png.gpi.d
ACM_proc.d: $$(call graphics-source,Pics/100000c.png)
ACM_proc.pdf ACM_proc._graphics: $$(call graphics-target,Pics/100000c.png)
-include Pics/1000000c.png.gpi.d
ACM_proc.d: $$(call graphics-source,Pics/1000000c.png)
ACM_proc.pdf ACM_proc._graphics: $$(call graphics-target,Pics/1000000c.png)
-include Pics/10000000c.png.gpi.d
ACM_proc.d: $$(call graphics-source,Pics/10000000c.png)
ACM_proc.pdf ACM_proc._graphics: $$(call graphics-target,Pics/10000000c.png)
