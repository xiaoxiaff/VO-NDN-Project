Virtual Organization
====================

This is the implementation of virtual organization using the VO-NDN library. 
All components in VO-NDN library will be implemented as an application and the experiment can be conducted based on this daemon.

* Compile the code

>./waf configure
>./waf

* How to run the daemon

Run NFD first:
>nfd-start

First set up the Attribute Authority:
>./build/bin/attribute_authority --name="/aaPrefix"

Then create the Producer:
>./build/bin/producer --pname="/Producer" --aname="/aaPrefix" --config="producerDataFile.txt"

The configure file is used to set up the mapping between data name and data content(file).

Then use data owner to set the policy for the specific Producer of specific data:
>./build/bin/data_owner --name="/DataOwner" --config="producerPolicy.txt"

Create consumer:
>./build/bin/consumer --name="/Consumer" --path="consumerCert" --tokenIssuerName="/TokenIssuer"

Create token issuer to load the certificate created by the consumer:
>./build/bin/token_issuer --name="/TokenIssuer" --config="tokenIssuerConsumer.txt"

Now you can type in the producer and the data you want in consumer terminal:
>/Producer,/data1