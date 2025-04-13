# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

import sys
from pathlib import Path

sys.path.append(str(Path('_ext').resolve()))

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'pars'
copyright = ' 2025 Giuseppe Roberti'
author = 'Giuseppe Roberti'
release = 'latest'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = ['breathe', 'sphinxnotes.strike', 'sphinx.domains.cpp', 'sphinx_fontawesome']

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

# html_theme = 'sphinx_rtd_theme'
html_theme = 'nature'
html_static_path = ['_static']

# Options for Breathe -----------------------------------------------------
# https://breathe.readthedocs.io/en/latest/directives.html#config-values

breathe_projects = {
    "pars": "../../out/build/linux-debug-llvm/docs/doxygen/xml"
}
breathe_default_project = "pars"
breathe_default_members = ('members', 'undoc-members', 'protected-members')
