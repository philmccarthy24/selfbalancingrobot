set(utility_files
  IConfigProvider.h
  JSONConfigProvider.h
  JSONConfigProvider.cpp
  Register.h
  Register.cpp
  ISBRFactory.h
  SBRProdFactory.h
  SBRProdFactory.cpp
)
add_library(utility ${utility_files})
target_include_directories(utility PRIVATE ./ ../ ../ahrs ../ahrs/sensors ../ahrs/algorithms ../coms)