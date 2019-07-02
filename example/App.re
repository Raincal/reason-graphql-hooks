let s = React.string;
let ate = React.array;

module AllPostsQueryConfig = [%graphql
  {|
    query allPosts($first: Int!, $skip: Int!) {
      allPosts(orderBy: createdAt_DESC, first: $first, skip: $skip) {
        id
        title
        votes
        url
        createdAt
      }
      _allPostsMeta {
        count
      }
    }
  |}
];

module AllPostsQuery = GraphqlHooks.CreateQuery(AllPostsQueryConfig);

[@react.component]
let make = () => {
  let (skip, setSkip) = React.useState(_ => 0);

  let (simple, _full, _refetch) =
    AllPostsQuery.use(
      ~variables=AllPostsQueryConfig.make(~skip, ~first=10, ())##variables,
      (),
    );

  <>
    {switch (simple) {
     | Loading => <div> "Loading"->s </div>
     | NoData => <div> "No Data"->s </div>
     | Data(data) =>
       let areMorePosts =
         Array.length(data##allPosts) < data##_allPostsMeta##count;

       <>
         <section>
           <ul>
             {data##allPosts
              |> Js.Array.mapi((post, index) =>
                   <li key={post##id}>
                     <div>
                       <span> {{(index + 1)->string_of_int ++ "."}->s} </span>
                       <a href={post##url}> {post##title->s} </a>
                     </div>
                   </li>
                 )
              |> ate}
           </ul>
           {areMorePosts
              ? <button onClick={_ => setSkip(_ => skip + 10)}>
                  "Show More"->s
                </button>
              : React.null}
         </section>
       </>;
     | Error(error) =>
       switch (error) {
       | GraphQLErrors(errors) =>
         errors
         |> Array.mapi((idx, error) =>
              <p key={idx->string_of_int}> {error##message->s} </p>
            )
         |> ate
       | HttpError(error) => <div> {error##body->s} </div>
       | FetchError(error) =>
         Js.log(error);
         <div> "fetch error"->s </div>;
       }
     }}
  </>;
};