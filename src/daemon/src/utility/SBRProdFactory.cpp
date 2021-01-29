
            SBRProdFactory::SBRProdFactory(const std::string& configFilePath) :
                m_configFilePath(configFilePath)
            {
            }
            SBRProdFactory::~SBRProdFactory()
            {
            }

            std::shared_ptr<IConfigSection> SBRProdFactory::CreateConfig() override;
            std::shared_ptr<coms::II2CDevice> SBRProdFactory::CreateI2CDevice(int deviceId) override;

            std::shared_ptr<ahrs::IAHRSDataSource> SBRProdFactory::CreateAHRSDataSource(const std::string& config) override;
            std::shared_ptr<ahrs::algorithms::IAHRSFusionAlgorithm> SBRProdFactory::CreateFusionAlgorithm(const std::string& config) override;
            std::shared_ptr<ahrs::ISensor> SBRProdFactory::CreateSensor(const std::string& config) override;