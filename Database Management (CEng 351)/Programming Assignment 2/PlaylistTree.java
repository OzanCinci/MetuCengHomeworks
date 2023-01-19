import java.util.ArrayList;

public class PlaylistTree {
	
	public PlaylistNode primaryRoot;		//root of the primary B+ tree
	public PlaylistNode secondaryRoot;	//root of the secondary B+ tree
	public int  maxElementCount ;
	public int middleElement ;
	public PlaylistTree(Integer order) {
		PlaylistNode.order = order;
		primaryRoot = new PlaylistNodePrimaryLeaf(null);
		primaryRoot.level = 0;
		secondaryRoot = new PlaylistNodeSecondaryLeaf(null);
		secondaryRoot.level = 0;
		maxElementCount = order * 2 + 1;
		middleElement = order;
	}
	
	public void addSong(CengSong song) {
		addPrimary(song);
		addSecondary(song);
	}

	public void addSecondary(CengSong song){
		// find where to insert
		PlaylistNodeSecondaryLeaf foundedLeaf = findSecondaryLeaf(song.genre());
		// insert the element into the leaf
		boolean bool = false;
		int calc;
		int i = 0;
		for (ArrayList<CengSong> el: foundedLeaf.getSongBucket()){
			calc = el.get(0).genre().toLowerCase().compareTo(song.genre());
			if ( 0 == calc ){
				el.add(song);
				bool = true;
				break;
			} else if (calc > 0)
				break;
			else
				i ++;
		}

		// if there is not such ArrayList, then create one.
		if (!bool){
			ArrayList<CengSong> newSongBucket = new ArrayList<CengSong>();
			newSongBucket.add(song);
			foundedLeaf.getSongBucket().add(i,newSongBucket);
		}

		// further calculations are needed in case of overflow in the foundedLeaf
		if (foundedLeaf.genreCount() == this.maxElementCount) {

			PlaylistNodeSecondaryIndex parent = (PlaylistNodeSecondaryIndex) foundedLeaf.getParent();
			// the foundedLeaf was the root of Secondary B+ Tree
			if (parent == null) {
				parent = new PlaylistNodeSecondaryIndex(null);
				parent.addChild(0, foundedLeaf);
				foundedLeaf.setParent(parent);
				secondaryRoot = parent;
			}

			// construct new leaf and redistribute foundedLeaf's songs
			PlaylistNodeSecondaryLeaf newLeaf = new PlaylistNodeSecondaryLeaf(parent);
			for(int j= maxElementCount-1; j >= this.middleElement ; j--){
				newLeaf.getSongBucket().add(0,foundedLeaf.songsAtIndex(j));
				foundedLeaf.getSongBucket().remove(j);
			}

			// parent shows new leaf and adjust the copy-up
			parent.deployNewChild(foundedLeaf, newLeaf);
			parent.addGenreInOrder(newLeaf.genreAtIndex(0));

			// handle tree order along the up way to the root
			recursivelyHandleSecondaryTreeOrder(parent);
		}
	}

	public void recursivelyHandleSecondaryTreeOrder(PlaylistNodeSecondaryIndex currentNode){
		if (currentNode.genreCount() < maxElementCount)
			return;

		PlaylistNodeSecondaryIndex parent = (PlaylistNodeSecondaryIndex) currentNode.getParent();
		// the foundedLeaf was the root of Secondary B+ Tree
		if (parent == null) {
			parent = new PlaylistNodeSecondaryIndex(null);
			parent.addChild(0, currentNode);
			currentNode.setParent(parent);
			secondaryRoot = parent;
		}

		// create a new node to redistribute
		PlaylistNodeSecondaryIndex newNode = new PlaylistNodeSecondaryIndex(parent);
		for(int j= maxElementCount-1; j >= this.middleElement ; j--){
			// relocate genre
			newNode.addGenreInOrder(currentNode.genreAtIndex(j));
			currentNode.removeGenreByGivenIndex(j);
			// handle parental issues
			newNode.addChild(0, currentNode.getChildrenAt(j+1));
			currentNode.getChildrenAt(j+1).setParent(newNode);
			currentNode.removeChildByGivenIndex(j+1);
		}

		// move up the middle
		// there is no need to copy-up
		// since it is internal node
		parent.addGenre(newNode.genreAtIndex(0));
		newNode.removeGenreByGivenIndex(0);

		// handle parental issues with new node
		parent.deployNewChild(currentNode,newNode);
		// recursively continue
		recursivelyHandleSecondaryTreeOrder(parent);
	}

	public void addPrimary(CengSong song){
		// find where to insert
		PlaylistNodePrimaryLeaf foundedLeaf = findLeaf(song.audioId());

		// insert the element into the leaf
		int i = 0;
		for ( CengSong cengSong: foundedLeaf.getSongs())
			if (cengSong.audioId() < song.audioId())
				i++;
		foundedLeaf.addSong(i,song);

		// further calculations are needed in case of overflow in the foundedLeaf
		if (foundedLeaf.songCount() == this.maxElementCount){
			PlaylistNodePrimaryIndex parent = (PlaylistNodePrimaryIndex) foundedLeaf.getParent();
			// the foundedLeaf was the root of Primary B+ Tree
			if (parent==null){
				parent = new PlaylistNodePrimaryIndex(null);
				parent.addChild(0,foundedLeaf);
				foundedLeaf.setParent(parent);
				primaryRoot = parent;
			}

			// construct new leaf and redistribute foundedLeaf's songs
			PlaylistNodePrimaryLeaf newLeaf = new PlaylistNodePrimaryLeaf(parent);
			for(int j= maxElementCount-1; j >= this.middleElement ; j--){
				newLeaf.addSong(0, foundedLeaf.songAtIndex(j));
				foundedLeaf.getSongs().remove(j);
			}

			// parent shows new leaf and adjust the copy-up
			parent.deployNewChild(foundedLeaf, newLeaf);
			parent.addAudioInOrder(newLeaf.songAtIndex(0).audioId());

			// handle tree order along the up way to the root
			recursivelyHandleTreeOrder(parent);
		}
	}

	public void recursivelyHandleTreeOrder(PlaylistNodePrimaryIndex currentNode){
		if (currentNode.audioIdCount() < maxElementCount)
			return;

		PlaylistNodePrimaryIndex parent = (PlaylistNodePrimaryIndex) currentNode.getParent();
		// the foundedLeaf was the root of Primary B+ Tree
		if (parent==null){
			parent = new PlaylistNodePrimaryIndex(null);
			parent.addChild(0,currentNode);
			primaryRoot = parent;
			currentNode.setParent(parent);
		}

		// create a new node to redistribute
		PlaylistNodePrimaryIndex newNode = new PlaylistNodePrimaryIndex(parent);
		for(int j= maxElementCount-1; j >= this.middleElement ; j--){
			// relocate audioID
			newNode.addAudioInOrder(currentNode.audioIdAtIndex(j));
			currentNode.removeAudioByGivenIndex(j);
			// handle parental issues
			newNode.addChild(0,currentNode.getChildrenAt(j+1));
			currentNode.getChildrenAt(j+1).setParent(newNode);
			currentNode.removeChildByGivenIndex(j+1);
		}

		// move up the middle
		// there is no need to copy-up
		// since it is internal node
		parent.addAudio(newNode.audioIdAtIndex(0));
		newNode.removeAudioByGivenIndex(0);

		// handle parental issues with new node
		parent.deployNewChild(currentNode,newNode);
		// recursively continue
		recursivelyHandleTreeOrder(parent);
	}
	
	public CengSong searchSong(Integer audioId) {
		PlaylistNode currentNode = primaryRoot;
		PlaylistNodePrimaryIndex currentIndexNode;
		PlaylistNodePrimaryLeaf tmp;
		int  i;
		int tabCount = 0;

		while (!currentNode.getType().equals(PlaylistNodeType.Leaf)) {
			currentIndexNode = (PlaylistNodePrimaryIndex) currentNode;

			System.out.println(duplicate(tabCount) + "<index>");
			for(Integer el: currentIndexNode.getAllAudioIds() )
				System.out.println(duplicate(tabCount) + el);
			System.out.println(duplicate(tabCount) + "</index>");

			for (i = 0; i < currentIndexNode.audioIdCount(); i++)
				if (audioId < currentIndexNode.audioIdAtIndex(i))
					break;
			currentNode = currentIndexNode.getChildrenAt(i);
			tabCount++;
		}
		tmp = (PlaylistNodePrimaryLeaf) currentNode;

		for(CengSong el: tmp.getSongs())
			if (el.audioId().equals(audioId)){
				System.out.println(duplicate(tabCount)+ "<data>");
				System.out.println(duplicate(tabCount)+"<record>"+el.fullName()+"</record>");
				System.out.println(duplicate(tabCount)+ "</data>");
				return el;
			}

		System.out.println("Could not find " + audioId);
		return null;
	}

	public void printPrimaryPlaylist() {
		printPrimaryPlaylistHelper(primaryRoot,0);
	}

	public String duplicate(int multiplier){
		StringBuilder result = new StringBuilder();
		while(multiplier-->0)
			result.append("\t");
		return result.toString();
	}

	public void printPrimaryPlaylistHelper(PlaylistNode currentNode, int depth){
		if (currentNode.getType().equals(PlaylistNodeType.Internal)){

			System.out.println(duplicate(depth)+"<index>");
			PlaylistNodePrimaryIndex currentIndex = (PlaylistNodePrimaryIndex) currentNode;
			for(Integer el: currentIndex.getAllAudioIds() )
				System.out.println(duplicate(depth)+el);
			System.out.println(duplicate(depth)+"</index>");

			for(PlaylistNode el: currentIndex.getAllChildren())
				printPrimaryPlaylistHelper(el,depth+1);

		} else {

			System.out.println(duplicate(depth)+"<data>");
			PlaylistNodePrimaryLeaf currentIndex = (PlaylistNodePrimaryLeaf) currentNode;
			for(CengSong el: currentIndex.getSongs() ){
				System.out.println(duplicate(depth)+"<record>"+el.fullName()+"</record>");
			}
			System.out.println(duplicate(depth)+"</data>");

		}
	}


	public void printSecondaryPlaylist() {
		printSecondaryPlaylist(secondaryRoot,0);
	}
	
	// HELPER FUNCTIONS //
	public void printSecondaryPlaylist(PlaylistNode currentNode, int depth){
		if (currentNode.getType().equals(PlaylistNodeType.Internal)){

			System.out.println(duplicate(depth)+"<index>");
			PlaylistNodeSecondaryIndex currentIndex = (PlaylistNodeSecondaryIndex) currentNode;
			for(String el: currentIndex.getGenres() )
				System.out.println(duplicate(depth)+el);
			System.out.println(duplicate(depth)+"</index>");

			for(PlaylistNode el: currentIndex.getChildren())
				printSecondaryPlaylist(el,depth+1);

		} else {

			System.out.println(duplicate(depth)+"<data>");
			PlaylistNodeSecondaryLeaf currentIndex = (PlaylistNodeSecondaryLeaf) currentNode;
			for(ArrayList<CengSong> el: currentIndex.getSongBucket() ){
				if (el.size()>0){
					System.out.println(duplicate(depth) + el.get(0).genre());
					for (CengSong cengSong: el)
						System.out.println(duplicate(depth+1)+"<record>"+cengSong.fullName()+"</record>");
				}
			}
			System.out.println(duplicate(depth)+"</data>");

		}
	}

	// findLeaf: find where the given id must be located.
	public PlaylistNodePrimaryLeaf findLeaf(int id){
		PlaylistNode currentNode = primaryRoot;
		PlaylistNodePrimaryIndex currentIndexNode;
		int  i;

		while (!currentNode.getType().equals(PlaylistNodeType.Leaf)) {
			currentIndexNode = (PlaylistNodePrimaryIndex) currentNode;
			for (i = 0; i < currentIndexNode.audioIdCount(); i++)
				if (id < currentIndexNode.audioIdAtIndex(i))
					break;
			currentNode = currentIndexNode.getChildrenAt(i);
		}

		return (PlaylistNodePrimaryLeaf) currentNode;
	}


	public PlaylistNodeSecondaryLeaf findSecondaryLeaf(String givenGenre){
		PlaylistNode currentNode = secondaryRoot;
		PlaylistNodeSecondaryIndex currentIndexNode;
		int i;

		while(!currentNode.getType().equals(PlaylistNodeType.Leaf)){
			currentIndexNode = (PlaylistNodeSecondaryIndex) currentNode;
			for(i=0; i < currentIndexNode.genreCount(); i++)
				if (0 > givenGenre.toLowerCase().compareTo(currentIndexNode.genreAtIndex(i).toLowerCase()))
					break;
			currentNode = currentIndexNode.getChildrenAt(i);
		}

		return (PlaylistNodeSecondaryLeaf) currentNode;
	}
}
