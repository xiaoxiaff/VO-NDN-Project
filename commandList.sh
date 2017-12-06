./build/bin/attribute_authority --name="/aaPrefix"
./build/bin/producer --pname="/Producer" --aname="/aaPrefix" --config="producerDataFile.txt"
./build/bin/consumer --name="/Consumer" --path="consumerCert" --tokenIssuerName="/TokenIssuer"
cin > /Producer,/data1
./build/bin/token_issuer --name="/TokenIssuer" --config="tokenIssuerConsumer.txt"
./build/bin/data_owner --name="/DataOwner" --config="producerPolicy.txt"
