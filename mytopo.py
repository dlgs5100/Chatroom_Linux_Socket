from mininet.topo import Topo

class MyTopo( Topo ):
    "Simple topology example."

    def __init__( self ):
        "Create custom topo."

        # Initialize topology
        Topo.__init__( self )

        # Add hosts and switches
        h1 = self.addHost( 'h1' )
        h2 = self.addHost( 'h2' )
		h3 = self.addHost( 'h3' )
        h4 = self.addHost( 'h4' )
        s1 = self.addSwitch( 's1' )

        # Add links
        self.addLink( h1, s1 )
	self.addLink( h2, s1 )
	self.addLink( h3, s1 )
	self.addLink( h4, s1 )

topos = { 'mytopo': ( lambda: MyTopo() ) }
