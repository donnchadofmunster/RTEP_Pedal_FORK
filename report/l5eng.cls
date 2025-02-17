%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% l5eng.cls - Updated to meet new formatting criteria
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{l5eng}[2025/01/24 Updated class for MEng Individual Project]

%%%%% Load Base Class
\LoadClass[a4paper,11pt]{report} % Default font size and paper size

%%%%% Page Layout
\usepackage[top=2cm, bottom=2cm, left=2cm, right=2cm]{geometry} % Set 2 cm margins
\pagestyle{plain} % Enable page numbering

%%%%% Line Spacing
\usepackage{setspace}
\onehalfspacing % Set 1.5 line spacing

%%%%% Required Packages
\usepackage{svg}
\usepackage{etoolbox}
\usepackage{graphicx}
\usepackage{subcaption}
\usepackage{amsmath, amsfonts, mathtools, amssymb}
\usepackage{float}
\usepackage{hyperref}
\usepackage{listings}
\usepackage{minted}
\usepackage{amsmath}
\usepackage{todonotes}
\usepackage{pdfpages}
\usemintedstyle{emacs}
\usepackage[style]{abstract}
\usepackage[compact]{titlesec} % Section heading formatting
\usepackage[titles]{tocloft}
\usepackage[backend=biber, style=ieee]{biblatex}
\usepackage[table,svgnames]{xcolor}
\usepackage[toc]{appendix}
%\usepackage{refcheck}

%%%%% Bibliography Setup
\addbibresource{References.bib} % Load the bibliography file

%%%%% Chapter Formatting
\usepackage{titlesec}
\titleformat{\chapter}[display]
  {\normalfont\huge\bfseries} % Chapter title styling
  {} % No prefix for chapter number
  {0pt} % No separation between chapter number and title
  {\filcenter} % Centered title
\titlespacing*{\chapter}{0pt}{-10pt}{20pt} % Adjust spacing

% Let us define a definition for unnumbered chapters
\titleformat{name=\chapter,numberless}
            [display]
            {\normalfont\huge\bfseries\secfont}
            {}
            {0pt}
            {}

%%%%% Start Chapters on New Pages
\renewcommand{\chapter}{\clearpage\thispagestyle{plain}\secdef\@chapter\@schapter}

%%%%% Font Settings
\usepackage{times} % Default to Times New Roman; swap with Arial or Helvetica if needed
\renewcommand{\rmdefault}{ptm} % Ensure Times is the serif font

%%%%% Style Adjustments
\renewcommand{\cftdot}{}
\renewcommand{\abstractnamefont}{\Large\sffamily}
\renewcommand{\abstitlestyle}[1]{{{\let\clearpage\relax \chapter*{#1}}}}

%%%%% Additional Options
\usepackage{fancyhdr} % For header and footer customization
\fancyhf{} % Clear all header/footer fields
\fancyfoot[C]{\thepage} % Centered page numbers in the footer
\renewcommand{\headrulewidth}{0pt} % No header line
\pagestyle{fancy}

%%%%% Prevent Widows and Orphans
\clubpenalty=10000
\widowpenalty=10000
\displaywidowpenalty=10000

%%%%% Include Hyperlinks
\hypersetup{
  colorlinks=false,
  linkcolor=blue,
  filecolor=magenta,  
  urlcolor=cyan,
  pdftitle={MEng Individual Project Report},
  pdfpagemode=FullScreen,
}

%%%%% Fix for Undefined Control Sequence Errors
\newcommand{\secfont}{} % Define \secfont to prevent undefined errors

%%%%% Additional Customization (if needed)

\definecolor{codegreen}{rgb}{0,0.6,0}
\definecolor{codegray}{rgb}{0.5,0.5,0.5}
\definecolor{codepurple}{rgb}{0.58,0,0.82}
\definecolor{backcolour}{rgb}{0.95,0.95,0.92}

\lstdefinestyle{mystyle}{
    backgroundcolor=\color{backcolour},   
    commentstyle=\color{codegreen},
    keywordstyle=\color{magenta},
    numberstyle=\tiny\color{codegray},
    stringstyle=\color{codepurple},
    basicstyle=\ttfamily\footnotesize,
    breakatwhitespace=false,         
    breaklines=true,                 
    captionpos=b,                    
    keepspaces=true,                 
    numbers=left,                    
    numbersep=5pt,                  
    showspaces=false,                
    showstringspaces=false,
    showtabs=false,                  
    tabsize=2
}

\lstset{style=mystyle}

%%%%% End of Class File
