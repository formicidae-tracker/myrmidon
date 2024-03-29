import os
import sys
# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))


# -- Project information -----------------------------------------------------

project = 'fort-myrmidon'
copyright = '2021, Université de Lausanne'
author = 'Alexandre Tuleu'


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    "breathe",
    "sphinx.ext.autodoc",
    "sphinx.ext.napoleon",
    "sphinx.ext.intersphinx",
    "sphinx.ext.mathjax",
]
intersphinx_mapping = {
    'python': ('https://docs.python.org/3', None),
    'numpy': ('http://docs.scipy.org/doc/numpy/', None),
}

breathe_default_project = "fort-myrmidon"

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_rtd_theme'
numfig = True

html_theme_options = {
    'collapse_navigation': True,
    'navigation_depth': 4,
}

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

html_context = {
    'display_github': True,
    'github_repo': 'formicidae-tracker/myrmidon',
    'github_version': 'master',
    'conf_py_path': '/docs/',
}

ci_build = True

if ci_build:
    html_extra_path = ['version_dropdown.js']
    html_context['version'] = 'placeholder'
    html_context['theme_display_version'] = True
    html_js_files = ['../version_dropdown.js']


rootpath = os.path.abspath(os.path.dirname(os.path.dirname(__file__)))
sys.path.append(os.path.join(rootpath, 'build/bindings/python/src'))
sys.path.append(os.path.join(rootpath, 'bindings/python/src'))
