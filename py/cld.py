import yade.config
if 'cldem' not in yade.config.features: raise ImportError("Compiled without the 'cldem' feature")
# intially empty, filled at initialization
