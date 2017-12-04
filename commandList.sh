./build/bin/attribute_authority --name="/aaPrefix"
./build/bin/consumer --name="/Consumer" --path="consumerCert" --tokenIssuerName="/TokenIssuer"
cin > /Producer,/data1
./build/bin/data_owner --name="/DataOwner" --config="producerPolicy.txt"
./build/bin/producer --pname="/Producer" --aname="/aaPrefix" --config="produerDataFile.txt"
./build/bin/token_issuer --name="/TokenIssuer" --config="tokenIssuerConsumer.txt"
