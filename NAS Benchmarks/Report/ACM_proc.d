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
-include Pics-Lab/BT.png.gpi.d
ACM_proc.d: $$(call graphics-source,Pics-Lab/BT.png)
ACM_proc.pdf ACM_proc._graphics: $$(call graphics-target,Pics-Lab/BT.png)
-include Pics-Lab/EP.png.gpi.d
ACM_proc.d: $$(call graphics-source,Pics-Lab/EP.png)
ACM_proc.pdf ACM_proc._graphics: $$(call graphics-target,Pics-Lab/EP.png)
-include Pics-Lab/FT.png.gpi.d
ACM_proc.d: $$(call graphics-source,Pics-Lab/FT.png)
ACM_proc.pdf ACM_proc._graphics: $$(call graphics-target,Pics-Lab/FT.png)
-include Pics-Lab/IS.png.gpi.d
ACM_proc.d: $$(call graphics-source,Pics-Lab/IS.png)
ACM_proc.pdf ACM_proc._graphics: $$(call graphics-target,Pics-Lab/IS.png)
-include Pics-Lab/LU.png.gpi.d
ACM_proc.d: $$(call graphics-source,Pics-Lab/LU.png)
ACM_proc.pdf ACM_proc._graphics: $$(call graphics-target,Pics-Lab/LU.png)
-include Pics-Lab/MG.png.gpi.d
ACM_proc.d: $$(call graphics-source,Pics-Lab/MG.png)
ACM_proc.pdf ACM_proc._graphics: $$(call graphics-target,Pics-Lab/MG.png)
-include Pics-Lab/SP.png.gpi.d
ACM_proc.d: $$(call graphics-source,Pics-Lab/SP.png)
ACM_proc.pdf ACM_proc._graphics: $$(call graphics-target,Pics-Lab/SP.png)
-include Pics-Pi/BT.png.gpi.d
ACM_proc.d: $$(call graphics-source,Pics-Pi/BT.png)
ACM_proc.pdf ACM_proc._graphics: $$(call graphics-target,Pics-Pi/BT.png)
-include Pics-Pi/EP.png.gpi.d
ACM_proc.d: $$(call graphics-source,Pics-Pi/EP.png)
ACM_proc.pdf ACM_proc._graphics: $$(call graphics-target,Pics-Pi/EP.png)
-include Pics-Pi/FT.png.gpi.d
ACM_proc.d: $$(call graphics-source,Pics-Pi/FT.png)
ACM_proc.pdf ACM_proc._graphics: $$(call graphics-target,Pics-Pi/FT.png)
-include Pics-Pi/IS.png.gpi.d
ACM_proc.d: $$(call graphics-source,Pics-Pi/IS.png)
ACM_proc.pdf ACM_proc._graphics: $$(call graphics-target,Pics-Pi/IS.png)
-include Pics-Pi/LU.png.gpi.d
ACM_proc.d: $$(call graphics-source,Pics-Pi/LU.png)
ACM_proc.pdf ACM_proc._graphics: $$(call graphics-target,Pics-Pi/LU.png)
-include Pics-Pi/MG.png.gpi.d
ACM_proc.d: $$(call graphics-source,Pics-Pi/MG.png)
ACM_proc.pdf ACM_proc._graphics: $$(call graphics-target,Pics-Pi/MG.png)
-include Pics-Pi/SP.png.gpi.d
ACM_proc.d: $$(call graphics-source,Pics-Pi/SP.png)
ACM_proc.pdf ACM_proc._graphics: $$(call graphics-target,Pics-Pi/SP.png)
