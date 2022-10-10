# ZeroTier

**Qu'est-ce que ZeroTier?**

ZeroTier est un éditeur de logiciels qui nous permettra de créer et de gérer des SDN (Software Defined Networks) pour intercommuniquer différents ordinateurs dans un réseau privé virtuel (VPN) facilement et rapidement. ZeroTier est l'un des moyens les plus simples de communiquer avec des ordinateurs sur Internet, sans avoir à créer nos propres tunnels VPN, nous laisserons donc derrière nous la complexité de le configurer nous-mêmes.

**Qu'est-ce qu'un réseau SDN?**

Le réseau SDN (software defined network) est une architecture réseau agile conçue pour rationaliser la gestion informatique et centraliser le contrôle, et ainsi aider les entreprises à s’adapter à la nature dynamique des applications d’aujourd’hui. Il sépare la gestion réseau de l’infrastructure réseau sous-jacente, permettant ainsi aux administrateurs de simplifier le provisioning des ressources réseau.

Dans un réseau SDN, un contrôleur d’applications logiciel gère le réseau et ses activités. Au lieu d’utiliser du matériel pour la prise en charge des services réseau, le SDN permet aux administrateurs réseau de virtualiser la connexion réseau physique.

**Qu'est-ce qui assure la sécurité des communications?**

En ce qui concerne la sécurité des communications avec ZeroTier, nous avons une clé publique asymétrique basée sur Curve25519 et Ed25519 pour la signature, elle utilise également Salsa20 ainsi que Poly1305 pour le cryptage symétrique des données et pour l'authentification des messages. Grâce à cette suite de chiffrement très similaire à celle utilisée par WireGuard, toutes nos communications entre ordinateurs seront cryptées.

**Quel est la différence entre les VPN standards?**

Une caractéristique très importante de ZeroTier est que les communications sont point à point. Contrairement aux VPN traditionnels où nous aurons un serveur ou un routeur central, dans ce cas, les messages sont envoyés directement d'un ordinateur à l'autre, sans qu'il soit nécessaire de passer par le nœud central. Cela nous donne une efficacité et une latence minimales, idéales pour avoir la meilleure expérience utilisateur.

Vue simplfiée d'un réseau ZeroTier

<figure><img src="../../../.gitbook/assets/zerotier (1).png" alt=""><figcaption></figcaption></figure>

Sources:\
[https://www.zerotier.com](https://www.zerotier.com/)\
[https://docs.zerotier.com](https://docs.zerotier.com/)\
[https://itigic.com/fr/zerotier-configure-secure-vpn-to-connect-computers-remotely/](https://itigic.com/fr/zerotier-configure-secure-vpn-to-connect-computers-remotely/)\
[https://www.citrix.com/fr-fr/solutions/app-delivery-and-security/what-is-software-defined-networking.html#:\~:text=Le réseau SDN](https://www.citrix.com/fr-fr/solutions/app-delivery-and-security/what-is-software-defined-networking.html)
