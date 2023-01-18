import java.util.ArrayList;

public class PlaylistNodeSecondaryIndex extends PlaylistNode {
	private ArrayList<String> genres;
	private ArrayList<PlaylistNode> children;

	public PlaylistNodeSecondaryIndex(PlaylistNode parent) {
		super(parent);
		genres = new ArrayList<String>();
		children = new ArrayList<PlaylistNode>();
		this.type = PlaylistNodeType.Internal;
	}
	
	public PlaylistNodeSecondaryIndex(PlaylistNode parent, ArrayList<String> genres, ArrayList<PlaylistNode> children) {
		super(parent);
		this.genres = genres;
		this.children = children;
		this.type = PlaylistNodeType.Internal;
	}
	
	// GUI Methods - Do not modify
	public ArrayList<PlaylistNode> getAllChildren()
	{
		return this.children;
	}
	
	public PlaylistNode getChildrenAt(Integer index) {
		
		return this.children.get(index);
	}
	

	public Integer genreCount()
	{
		return this.genres.size();
	}
	
	public String genreAtIndex(Integer index) {
		if(index >= this.genreCount() || index < 0) {
			return "Not Valid Index!!!";
		}
		else {
			return this.genres.get(index);
		}
	}
	
	
	// Extra functions if needed

	public  ArrayList<PlaylistNode> getChildren(){
		return this.children;
	}

	public  ArrayList<String> getGenres(){
		return this.genres;
	}

	// insert a new child
	public void addChild(int index, PlaylistNode child){
		this.children.add(index,child);
	}

	// deploy a child after a given -existing- child
	public void deployNewChild(PlaylistNode givenChild, PlaylistNode newChild){
		int index;
		for(index=0; index< this.children.size() ;index++)
			if (this.children.get(index).equals(givenChild)){
				addChild(index+1,newChild);
				break;
			}
	}

	// add genre
	public void addGenre(String genre){
		this.genres.add(genre);
	}

	// add genre in order
	public void addGenreInOrder(String genre){
		int i = 0;
		while(i < this.genres.size() && 0 < genre.toLowerCase().compareTo(this.genres.get(i).toLowerCase()))
			i++;
		this.genres.add(i, genre);
	}

	// remove genre by given index
	public void removeGenreByGivenIndex(int index){this.genres.remove(index);}

	// remove child by given index
	public void removeChildByGivenIndex(int index){ this.children.remove(index);}
}
